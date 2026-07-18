#ifndef CAMPAIGN_H
#define CAMPAIGN_H

// Campaign day state (Phase C). Day = campaign node; Shift = timed play for that day.
// Session memory only — no SRAM save required.
namespace campaign
{

// Finite early campaign length. Day index is 1…max_days.
inline constexpr int max_days = 5;

// Per-day spawn + shift length (C-03). Day 1 matches Phase A baseline; later days tighter.
// Tweak this table only — spawner / shift scene read via difficulty_for_day().
struct day_difficulty
{
    int first_spawn_seconds;
    int interval_seconds;
    int interval_shrink_seconds;
    int min_interval_seconds;
    int shift_duration_seconds;
};

// 1-based day → params. Out-of-range clamps to [1, max_days].
[[nodiscard]] day_difficulty difficulty_for_day(int day);

// Current campaign day (1-based). Starts at 1 after reset / boot.
[[nodiscard]] int current_day();

// New run: back to Day 1.
void reset();

// Pass path (C-04): move to the next day. No-op when already on max_days
// (final-day pass uses reset() after the campaign-complete results screen).
void advance();

// Debug / playtest: jump to a day in 1…max_days (clamped).
void set_day(int day);

}

#endif
