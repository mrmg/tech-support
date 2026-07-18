#include "ticket.h"

#include "bn_random.h"

#include "campaign.h"
#include "sfx.h"

namespace ticket
{

namespace
{

// Shared across shifts so desk/issue rolls are not reset to the same sequence each day.
// Seeded by Butano defaults; title/day-intro tick_rng() mixes in wait-frame entropy.
bn::random& spawn_rng()
{
    static bn::random rng;
    return rng;
}

[[nodiscard]] int clamp_day(int day)
{
    if(day < 1)
    {
        return 1;
    }

    if(day > campaign::max_days)
    {
        return campaign::max_days;
    }

    return day;
}

}

namespace director
{

pool_weights pool_weights_for_day(int day)
{
    switch(clamp_day(day))
    {
    case 1:
        // Day 1 — reboot only.
        return pool_weights{ 100, 0, 0 };

    case 2:
        // Day 2 — reboot 70% / toner 30%.
        return pool_weights{ 70, 30, 0 };

    case 3:
    case 4:
        // Days 3–4 — reboot 70% / toner 20% / PSU 10%. Server stays out of pool.
        return pool_weights{ 70, 20, 10 };

    default:
        return pool_weights{ 100, 0, 0 };
    }
}

bool has_pool_teaching_guarantee(int day)
{
    // Day 4 teaching is the global outage (J-07), scheduled outside this pool.
    const int clamped = clamp_day(day);
    return clamped >= 1 && clamped <= 3;
}

bool schedules_global_outage(int day)
{
    return clamp_day(day) == 4;
}

type pool_teaching_kind(int day)
{
    switch(clamp_day(day))
    {
    case 1:
        return type::reboot;

    case 2:
        return type::needs_toner;

    case 3:
        return type::needs_psu;

    default:
        // Day 4+ — callers must use has_pool_teaching_guarantee first.
        return type::reboot;
    }
}

target_kind target_kind_for_issue(type issue_type)
{
    switch(issue_type)
    {
    case type::reboot:
    case type::needs_psu:
        return target_kind::computer_desk;

    case type::needs_toner:
        return target_kind::printer;

    case type::needs_server_reset:
        return target_kind::global_server;

    default:
        return target_kind::computer_desk;
    }
}

type issue_type_from_pool_roll(const pool_weights& weights, int roll)
{
    const int total = weights.total();

    if(total <= 0)
    {
        return type::reboot;
    }

    // Normalize out-of-range rolls so helpers stay safe in tests.
    if(roll < 0)
    {
        roll = 0;
    }
    else if(roll >= total)
    {
        roll = total - 1;
    }

    if(roll < weights.reboot)
    {
        return type::reboot;
    }

    roll -= weights.reboot;

    if(roll < weights.toner)
    {
        return type::needs_toner;
    }

    return type::needs_psu;
}

}

namespace spawn
{

void tick_rng()
{
    spawn_rng().update();
}

}

bn::string_view issue_label(type issue_type)
{
    // J-09: notepad / results use these exact design names.
    switch(issue_type)
    {
    case type::reboot:
        return "PC reboot";

    case type::needs_toner:
        return "Printer toner";

    case type::needs_psu:
        return "PC PSU";

    case type::needs_server_reset:
        return "Server outage";

    default:
        {
            // Exhaustive for known kinds; unknown → "?" so notepad never blanks.
            return "?";
        }
    }
}

bool requires_part(type issue_type)
{
    switch(issue_type)
    {
    case type::needs_toner:
    case type::needs_psu:
        return true;

    case type::reboot:
    case type::needs_server_reset:
        return false;

    default:
        return false;
    }
}

carry::part required_part(type issue_type)
{
    switch(issue_type)
    {
    case type::needs_toner:
        return carry::part::toner;

    case type::needs_psu:
        return carry::part::psu;

    case type::reboot:
    case type::needs_server_reset:
        return carry::part::none;

    default:
        return carry::part::none;
    }
}

bool requires_server_reset(type issue_type)
{
    switch(issue_type)
    {
    case type::needs_server_reset:
        return true;

    case type::reboot:
    case type::needs_toner:
    case type::needs_psu:
        return false;

    default:
        return false;
    }
}

spawner::spawner() :
    spawner(spawn::params{})
{
}

spawner::spawner(const spawn::params& params) :
    _params(params),
    _frames_until_next_spawn(params.first_spawn_frames),
    _spawned_count(0),
    _fixed_count(0),
    _classified(false),
    _global_outage_spawned(false)
{
}

void spawner::update()
{
    _advance_urgency();

    if(_frames_until_next_spawn < 0)
    {
        return;
    }

    if(_frames_until_next_spawn > 0)
    {
        --_frames_until_next_spawn;

        if(_frames_until_next_spawn > 0)
        {
            return;
        }
    }

    _try_spawn();
}

bn::span<const instance> spawner::open_tickets() const
{
    return bn::span<const instance>(_open.data(), _open.size());
}

int spawner::open_count() const
{
    return _open.size();
}

int spawner::fixed_count() const
{
    return _fixed_count;
}

int spawner::spawned_count() const
{
    return _spawned_count;
}

bool spawner::desk_has_open_ticket(int target_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.target_id == target_id)
        {
            return true;
        }
    }

    return false;
}

type spawner::issue_type_for_desk(int target_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.target_id == target_id)
        {
            return entry.issue_type;
        }
    }

    return type::reboot;
}

int spawner::urgency_for_desk(int target_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.target_id == target_id)
        {
            return entry.urgency;
        }
    }

    return 0;
}

bn::span<const history_entry> spawner::history() const
{
    return bn::span<const history_entry>(_history.data(), _history.size());
}

bool spawner::is_classified() const
{
    return _classified;
}

void spawner::clear_desk(int target_id)
{
    for(int index = 0; index < _open.size(); ++index)
    {
        if(_open[index].open && _open[index].target_id == target_id)
        {
            _mark_history_fixed(target_id);
            _open.erase(_open.begin() + index);
            _urgency_frames.erase(_urgency_frames.begin() + index);
            ++_fixed_count;
            return;
        }
    }
}

void spawner::clear_server_reset_tickets()
{
    // One global incident → one fixed count (never one-per-desk).
    clear_desk(global_server_target_id);
}

bool spawner::has_open_server_reset() const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.issue_type == type::needs_server_reset)
        {
            return true;
        }
    }

    return false;
}

int spawner::server_outage_urgency() const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.issue_type == type::needs_server_reset)
        {
            return entry.urgency;
        }
    }

    return 0;
}

void spawner::classify_at_bell()
{
    if(_classified)
    {
        return;
    }

    for(history_entry& entry : _history)
    {
        if(entry.result == outcome::pending)
        {
            // Still open at the bell → failed (not fixed in time). Not a mid-shift expiry.
            entry.result = outcome::failed;
        }
    }

    _classified = true;
}

void spawner::_mark_history_fixed(int target_id)
{
    // One open ticket per target ⇒ at most one pending history row for that id.
    for(int index = _history.size() - 1; index >= 0; --index)
    {
        history_entry& entry = _history[index];

        if(entry.target_id == target_id && entry.result == outcome::pending)
        {
            entry.result = outcome::fixed;
            return;
        }
    }
}

void spawner::_advance_urgency()
{
    for(int index = 0; index < _open.size(); ++index)
    {
        instance& entry = _open[index];

        if(! entry.open || entry.urgency >= urgency::max_level)
        {
            continue;
        }

        ++_urgency_frames[index];

        if(_urgency_frames[index] < urgency::frames_per_level)
        {
            continue;
        }

        _urgency_frames[index] = 0;
        ++entry.urgency;
        // Soft cap only — ticket stays open and fixable at max_level.
    }
}

int spawner::_pick_free_desk()
{
    int free_desks[desk::count];
    int free_count = 0;

    for(int desk_id = 0; desk_id < desk::count; ++desk_id)
    {
        if(! desk_has_open_ticket(desk_id))
        {
            free_desks[free_count] = desk_id;
            ++free_count;
        }
    }

    if(free_count <= 0)
    {
        return -1;
    }

    return free_desks[spawn_rng().get_unbiased_int(free_count)];
}

int spawner::_pick_free_printer()
{
    int free_printers[printer::count];
    int free_count = 0;

    for(int index = 0; index < printer::count; ++index)
    {
        const int id = printer::target_id(index);

        if(! desk_has_open_ticket(id))
        {
            free_printers[free_count] = id;
            ++free_count;
        }
    }

    if(free_count <= 0)
    {
        return -1;
    }

    return free_printers[spawn_rng().get_unbiased_int(free_count)];
}

type spawner::_pick_issue_type()
{
    const int day = campaign::current_day();

    // Day 4 teaching: one global outage on the first spawn (outside the random pool).
    if(_spawned_count == 0 && director::schedules_global_outage(day) && ! _global_outage_spawned)
    {
        return type::needs_server_reset;
    }

    // First pool spawn of the day: guaranteed teaching kind (Days 1–3).
    if(_spawned_count == 0 && director::has_pool_teaching_guarantee(day))
    {
        return director::pool_teaching_kind(day);
    }

    const director::pool_weights weights = director::pool_weights_for_day(day);
    const int roll = spawn_rng().get_unbiased_int(weights.total());
    return director::issue_type_from_pool_roll(weights, roll);
}

int spawner::_pick_target_for_issue(type issue_type)
{
    switch(director::target_kind_for_issue(issue_type))
    {
    case target_kind::computer_desk:
        // Reboot + PSU bind to free computer desks only (J-06).
        return _pick_free_desk();

    case target_kind::printer:
        // Toner never binds to a computer desk (J-03).
        return _pick_free_printer();

    case target_kind::global_server:
        // One office-wide incident — not one ticket per desk (J-07).
        return global_server_target_id;

    default:
        return _pick_free_desk();
    }
}

void spawner::_schedule_next_spawn()
{
    // After N spawns, interval = interval - (N-1)*shrink, floored at min.
    int next_frames = _params.interval_frames - (_spawned_count - 1) * _params.interval_shrink_frames;

    if(next_frames < _params.min_interval_frames)
    {
        next_frames = _params.min_interval_frames;
    }

    _frames_until_next_spawn = next_frames;
}

void spawner::_try_spawn()
{
    if(_open.full())
    {
        // All desk/printer/outage slots busy; keep polling until a slot frees.
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    if(_history.full())
    {
        // History full — stop scheduling further spawns (should not hit at default length).
        _frames_until_next_spawn = -1;
        return;
    }

    // Kind first, then eligible target — never derive kind from a desk/printer id.
    const type issue_type = _pick_issue_type();

    // At most one outage per Day 4 shift; never stack simultaneous outages.
    if(issue_type == type::needs_server_reset &&
       (_global_outage_spawned || has_open_server_reset()))
    {
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    // While the office-wide outage is active, pause normal computer incidents (J-07).
    // Existing non-server tickets may stay open; printer toner can still spawn.
    if(has_open_server_reset() &&
       director::target_kind_for_issue(issue_type) == target_kind::computer_desk)
    {
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    const int target_id = _pick_target_for_issue(issue_type);

    if(target_id < 0)
    {
        // No free target of the right kind (e.g. both printers busy for toner).
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    _open.push_back(instance{target_id, issue_type, urgency::initial_level, true});
    _urgency_frames.push_back(0);
    _history.push_back(history_entry{target_id, issue_type, outcome::pending});
    ++_spawned_count;

    if(issue_type == type::needs_server_reset)
    {
        _global_outage_spawned = true;
    }

    _schedule_next_spawn();
    sfx::play_ticket_spawn();
}

}
