#ifndef TICKET_H
#define TICKET_H

#include "bn_span.h"
#include "bn_string_view.h"
#include "bn_vector.h"

#include "desk.h"
#include "shift.h"

// Ticket model + spawn curve for the shift (Phase A: reboot only).
namespace ticket
{

enum class type
{
    reboot,
};

// End-of-shift classification only (Phase B). pending until the bell; never mid-shift fail.
enum class outcome
{
    pending,
    fixed,
    failed,
};

struct instance
{
    int desk_id;
    type issue_type;
    int urgency;
    bool open;
};

// One spawn log row for results UI (desk + issue + ✓/✗ after classify_at_bell).
struct history_entry
{
    int desk_id;
    type issue_type;
    outcome result;
};

// Soft urgency (tweakable). Rises while open; never fails/removes mid-shift (Phase B accounts ✗ at bell).
namespace urgency
{
    inline constexpr int max_level = 9;
    inline constexpr int initial_level = 0;

    // Time for an open ticket to climb one urgency step (paused while notepad is open).
    inline constexpr int seconds_per_level = 10;
    inline constexpr int frames_per_level = seconds_per_level * shift::frames_per_second;
}

// Spawn timing (tweakable). No tickets at t=0; first spawn after first_spawn_seconds.
namespace spawn
{
    inline constexpr int first_spawn_seconds = 5;

    // Gap after the first ticket; each later spawn shrinks by interval_shrink_seconds
    // down to min_interval_seconds (gentle pressure curve across the shift).
    inline constexpr int interval_seconds = 18;
    inline constexpr int interval_shrink_seconds = 2;
    inline constexpr int min_interval_seconds = 8;

    inline constexpr int first_spawn_frames = first_spawn_seconds * shift::frames_per_second;
    inline constexpr int interval_frames = interval_seconds * shift::frames_per_second;
    inline constexpr int interval_shrink_frames = interval_shrink_seconds * shift::frames_per_second;
    inline constexpr int min_interval_frames = min_interval_seconds * shift::frames_per_second;
}

// Max one open ticket per desk.
inline constexpr int max_open = desk::count;

// Spawn history capacity (covers a full shift even with frequent clears).
inline constexpr int max_history = 32;

// Short issue line for notepad / results (shared label for a ticket type).
[[nodiscard]] bn::string_view issue_label(type issue_type);

class spawner
{
public:
    spawner();

    // Advance one shift frame: raise soft urgency on open tickets, then maybe spawn.
    // Does not fail, expire, or remove tickets for urgency (fixes only via clear_desk).
    void update();

    [[nodiscard]] bn::span<const instance> open_tickets() const;
    [[nodiscard]] int open_count() const;
    // Tickets cleared via hold-to-reboot this shift (Phase A summary; not a pass gate).
    [[nodiscard]] int fixed_count() const;
    [[nodiscard]] int spawned_count() const;
    [[nodiscard]] bool desk_has_open_ticket(int desk_id) const;
    // Urgency of the open ticket on desk_id, or 0 if none.
    [[nodiscard]] int urgency_for_desk(int desk_id) const;

    // Spawn log for results UI (outcomes pending until classify_at_bell).
    [[nodiscard]] bn::span<const history_entry> history() const;
    [[nodiscard]] bool is_classified() const;

    // Close/remove the open ticket bound to desk_id (no-op if none). Counts as fixed.
    void clear_desk(int desk_id);

    // Shift timer hit zero: pending → failed; already cleared stay fixed. Call once at the bell.
    void classify_at_bell();

private:
    bn::vector<instance, max_open> _open;
    // Frames accumulated toward the next urgency step, parallel to _open.
    bn::vector<int, max_open> _urgency_frames;
    bn::vector<history_entry, max_history> _history;
    int _frames_until_next_spawn;
    int _spawned_count;
    int _fixed_count;
    bool _classified;

    [[nodiscard]] int _find_free_desk() const;
    void _schedule_next_spawn();
    void _try_spawn();
    void _advance_urgency();
    void _mark_history_fixed(int desk_id);
};

}

#endif
