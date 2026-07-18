#include "ticket.h"

namespace ticket
{

spawner::spawner() :
    _frames_until_next_spawn(spawn::first_spawn_frames),
    _spawned_count(0),
    _fixed_count(0)
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

void spawner::clear_desk(int desk_id)
{
    for(int index = 0; index < _open.size(); ++index)
    {
        if(_open[index].open && _open[index].desk_id == desk_id)
        {
            _open.erase(_open.begin() + index);
            _urgency_frames.erase(_urgency_frames.begin() + index);
            ++_fixed_count;
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

int spawner::_find_free_desk() const
{
    for(int desk_id = 0; desk_id < desk::count; ++desk_id)
    {
        if(! desk_has_open_ticket(desk_id))
        {
            return desk_id;
        }
    }

    return -1;
}

void spawner::_schedule_next_spawn()
{
    // After N spawns, interval = interval - (N-1)*shrink, floored at min.
    int next_frames = spawn::interval_frames - (_spawned_count - 1) * spawn::interval_shrink_frames;

    if(next_frames < spawn::min_interval_frames)
    {
        next_frames = spawn::min_interval_frames;
    }

    _frames_until_next_spawn = next_frames;
}

void spawner::_try_spawn()
{
    if(_open.full())
    {
        // All desks busy; keep polling until a slot frees (fix clears in A-10).
        _frames_until_next_spawn = spawn::min_interval_frames;
        return;
    }

    const int desk_id = _find_free_desk();

    if(desk_id < 0)
    {
        _frames_until_next_spawn = spawn::min_interval_frames;
        return;
    }

    _open.push_back(instance{desk_id, type::reboot, urgency::initial_level, true});
    _urgency_frames.push_back(0);
    ++_spawned_count;
    _schedule_next_spawn();
}

}
