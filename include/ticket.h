#ifndef TICKET_H
#define TICKET_H

#include "bn_span.h"
#include "bn_string_view.h"
#include "bn_vector.h"

#include "carry.h"
#include "desk.h"
#include "printer.h"
#include "shift.h"

// Ticket model + spawn curve (reboot, needs-part, cross-room server reset).
namespace ticket
{

// Data-driven issue kinds. needs_toner/psu require the matching carry::part.
// needs_server_reset clears only via server-room rack (E-03), not desk hold-A.
enum class type
{
    reboot,
    needs_toner,
    needs_psu,
    needs_server_reset,
};

// Where an incident binds in the world. Kind pick and target pick stay separate
// so computer / printer / global outages cannot be confused (J-02 / J-03 / J-07).
enum class target_kind
{
    computer_desk,
    printer,
    global_server,
};

// Day-gated random pool (J-02). Server outage is never in this pool — Day 4
// schedules it separately (J-07). Weights are percent points (sum 100).
namespace director
{
    struct pool_weights
    {
        int reboot;
        int toner;
        int psu;

        [[nodiscard]] constexpr int total() const
        {
            return reboot + toner + psu;
        }
    };

    // 1-based campaign day → pool weights. Out-of-range clamps like campaign days.
    [[nodiscard]] pool_weights pool_weights_for_day(int day);

    // Days 1–3 force their teaching kind on the first pool spawn. Day 4 teaching
    // (global outage) is scripted outside the pool, so this is false there.
    [[nodiscard]] bool has_pool_teaching_guarantee(int day);

    // Day 4 schedules one office-wide outage outside the random pool (J-07).
    [[nodiscard]] bool schedules_global_outage(int day);

    // Teaching kind for days with a pool guarantee (reboot / toner / PSU).
    // Undefined for Day 4 — check has_pool_teaching_guarantee first.
    [[nodiscard]] type pool_teaching_kind(int day);

    // Map issue type → target class (desk vs printer vs global).
    [[nodiscard]] target_kind target_kind_for_issue(type issue_type);

    // Deterministic weighted pick from a pool roll in [0, weights.total()).
    [[nodiscard]] type issue_type_from_pool_roll(const pool_weights& weights, int roll);
}

// End-of-shift classification only (Phase B). pending until the bell; never mid-shift fail.
enum class outcome
{
    pending,
    fixed,
    failed,
};

struct instance
{
    // Desk index, printer::target_id(i), or global_server_target_id (J-07).
    int target_id;
    type issue_type;
    int urgency;
    bool open;
};

// One spawn log row for results UI (target + issue + ✓/✗ after classify_at_bell).
struct history_entry
{
    int target_id;
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
// Day 1 baseline (J-01); live shifts use campaign::day_difficulty (C-03 / campaign.cpp).
namespace spawn
{
    inline constexpr int first_spawn_seconds = 5;

    // Gap after the first ticket; each later spawn shrinks by interval_shrink_seconds
    // down to min_interval_seconds (gentle pressure curve across the shift).
    inline constexpr int interval_seconds = 12;
    inline constexpr int interval_shrink_seconds = 1;
    inline constexpr int min_interval_seconds = 8;

    inline constexpr int first_spawn_frames = first_spawn_seconds * shift::frames_per_second;
    inline constexpr int interval_frames = interval_seconds * shift::frames_per_second;
    inline constexpr int interval_shrink_frames = interval_shrink_seconds * shift::frames_per_second;
    inline constexpr int min_interval_frames = min_interval_seconds * shift::frames_per_second;

    // Runtime curve fed into spawner (Day 1 == baseline above).
    struct params
    {
        int first_spawn_frames = spawn::first_spawn_frames;
        int interval_frames = spawn::interval_frames;
        int interval_shrink_frames = spawn::interval_shrink_frames;
        int min_interval_frames = spawn::min_interval_frames;
    };

    [[nodiscard]] inline params from_seconds(int first_spawn_sec, int interval_sec, int shrink_sec,
                                             int min_interval_sec)
    {
        return params{
            first_spawn_sec * shift::frames_per_second,
            interval_sec * shift::frames_per_second,
            shrink_sec * shift::frames_per_second,
            min_interval_sec * shift::frames_per_second,
        };
    }

    // Advance shared bn::random once (call from title / day-intro waits for entropy).
    void tick_rng();
}

// Global outage binds here — not a desk or printer (J-07).
inline constexpr int global_server_target_id = desk::count + printer::count;

[[nodiscard]] constexpr bool is_global_server_target(int target_id)
{
    return target_id == global_server_target_id;
}

// Max one open ticket per desk/printer, plus one global server outage slot.
inline constexpr int max_open = desk::count + printer::count + 1;

// Spawn history capacity (covers a full shift even with frequent clears).
inline constexpr int max_history = 32;

// Notepad / results issue names: "PC reboot", "Printer toner", "PC PSU", "Server outage".
[[nodiscard]] bn::string_view issue_label(type issue_type);

// True for needs_toner / needs_psu (hold-to-install consumes required_part).
[[nodiscard]] bool requires_part(type issue_type);

// Part required to clear a needs-* ticket; carry::part::none for reboot / server reset.
[[nodiscard]] carry::part required_part(type issue_type);

// True for needs_server_reset (desk hold-A cannot clear; rack reset can).
[[nodiscard]] bool requires_server_reset(type issue_type);

class spawner
{
public:
    // Default ctor keeps Day 1 / Phase A baseline (spawn::* constants).
    spawner();
    explicit spawner(const spawn::params& params);

    // Advance one shift frame: raise soft urgency on open tickets, then maybe spawn.
    // Does not fail, expire, or remove tickets for urgency (fixes only via clear_desk).
    void update();

    [[nodiscard]] bn::span<const instance> open_tickets() const;
    [[nodiscard]] int open_count() const;
    // Tickets cleared via hold-to-reboot this shift (Phase A summary; not a pass gate).
    [[nodiscard]] int fixed_count() const;
    [[nodiscard]] int spawned_count() const;
    // True if target_id (desk or printer) already has an open ticket.
    [[nodiscard]] bool desk_has_open_ticket(int target_id) const;
    // Issue kind of the open ticket on target_id, or reboot if none.
    [[nodiscard]] type issue_type_for_desk(int target_id) const;
    // Urgency of the open ticket on target_id, or 0 if none.
    [[nodiscard]] int urgency_for_desk(int target_id) const;

    // Spawn log for results UI (outcomes pending until classify_at_bell).
    [[nodiscard]] bn::span<const history_entry> history() const;
    [[nodiscard]] bool is_classified() const;

    // Close/remove the open ticket bound to target_id (no-op if none). Counts as fixed.
    void clear_desk(int target_id);

    // Clear the open global outage (rack complete). Counts as one fixed incident.
    void clear_server_reset_tickets();

    // True if the office-wide server outage is active (used to re-arm the rack).
    [[nodiscard]] bool has_open_server_reset() const;

    // Urgency of the active global outage, or 0 if none (shared desk error flash).
    [[nodiscard]] int server_outage_urgency() const;

    // Shift timer hit zero: pending → failed; already cleared stay fixed. Call once at the bell.
    void classify_at_bell();

private:
    bn::vector<instance, max_open> _open;
    // Frames accumulated toward the next urgency step, parallel to _open.
    bn::vector<int, max_open> _urgency_frames;
    bn::vector<history_entry, max_history> _history;
    spawn::params _params;
    int _frames_until_next_spawn;
    int _spawned_count;
    int _fixed_count;
    bool _classified;
    // Day 4: at most one outage spawn per shift (guards duplicates after clear).
    bool _global_outage_spawned;

    // Uniform pick among desks with no open ticket; -1 if none free.
    [[nodiscard]] int _pick_free_desk();
    // Uniform pick among printers with no open ticket; -1 if none free.
    [[nodiscard]] int _pick_free_printer();
    // Day-gated teaching guarantee / Day 4 outage, then weighted pool (never server).
    [[nodiscard]] type _pick_issue_type();
    // Target for a chosen kind (desk / printer / global_server_target_id).
    [[nodiscard]] int _pick_target_for_issue(type issue_type);
    void _schedule_next_spawn();
    void _try_spawn();
    void _advance_urgency();
    void _mark_history_fixed(int target_id);
};

}

#endif
