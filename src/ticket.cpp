#include "ticket.h"

#include "bn_random.h"

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
    switch(issue_type)
    {
    case type::reboot:
        return "reboot";

    case type::needs_toner:
        return "needs toner";

    case type::needs_psu:
        return "needs PSU";

    case type::needs_server_reset:
        return "reset server";

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
    _classified(false)
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

bool spawner::desk_has_open_ticket(int desk_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.desk_id == desk_id)
        {
            return true;
        }
    }

    return false;
}

type spawner::issue_type_for_desk(int desk_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.desk_id == desk_id)
        {
            return entry.issue_type;
        }
    }

    return type::reboot;
}

int spawner::urgency_for_desk(int desk_id) const
{
    for(const instance& entry : _open)
    {
        if(entry.open && entry.desk_id == desk_id)
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

void spawner::clear_desk(int desk_id)
{
    for(int index = 0; index < _open.size(); ++index)
    {
        if(_open[index].open && _open[index].desk_id == desk_id)
        {
            _mark_history_fixed(desk_id);
            _open.erase(_open.begin() + index);
            _urgency_frames.erase(_urgency_frames.begin() + index);
            ++_fixed_count;
            return;
        }
    }
}

void spawner::clear_server_reset_tickets()
{
    for(int index = _open.size() - 1; index >= 0; --index)
    {
        if(! _open[index].open || _open[index].issue_type != type::needs_server_reset)
        {
            continue;
        }

        const int desk_id = _open[index].desk_id;
        _mark_history_fixed(desk_id);
        _open.erase(_open.begin() + index);
        _urgency_frames.erase(_urgency_frames.begin() + index);
        ++_fixed_count;
    }
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

void spawner::_mark_history_fixed(int desk_id)
{
    // One open ticket per desk ⇒ at most one pending history row for that desk.
    for(int index = _history.size() - 1; index >= 0; --index)
    {
        history_entry& entry = _history[index];

        if(entry.desk_id == desk_id && entry.result == outcome::pending)
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

type spawner::_pick_issue_type()
{
    // Same proportions as the old 5-cycle (reboot×2, toner, server, PSU) — weighted, not sequenced.
    constexpr int weight_reboot = 2;
    constexpr int weight_toner = 1;
    constexpr int weight_server = 1;
    constexpr int weight_psu = 1;
    constexpr int weight_total = weight_reboot + weight_toner + weight_server + weight_psu;

    int roll = spawn_rng().get_unbiased_int(weight_total);

    if(roll < weight_reboot)
    {
        return type::reboot;
    }

    roll -= weight_reboot;

    if(roll < weight_toner)
    {
        return type::needs_toner;
    }

    roll -= weight_toner;

    if(roll < weight_server)
    {
        return type::needs_server_reset;
    }

    return type::needs_psu;
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
        // All desks busy; keep polling until a slot frees (fix clears in A-10).
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    if(_history.full())
    {
        // History full — stop scheduling further spawns (should not hit at default length).
        _frames_until_next_spawn = -1;
        return;
    }

    const int desk_id = _pick_free_desk();

    if(desk_id < 0)
    {
        _frames_until_next_spawn = _params.min_interval_frames;
        return;
    }

    const type issue_type = _pick_issue_type();

    _open.push_back(instance{desk_id, issue_type, urgency::initial_level, true});
    _urgency_frames.push_back(0);
    _history.push_back(history_entry{desk_id, issue_type, outcome::pending});
    ++_spawned_count;
    _schedule_next_spawn();
    sfx::play_ticket_spawn();
}

}
