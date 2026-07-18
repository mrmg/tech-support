#ifndef REPUTATION_H
#define REPUTATION_H

// Session office anger / reputation pressure (Phase G). Session memory only — no SRAM.
// G-01: persist across days; worsen on day fail; slight relief on strong pass.
// G-03: shift_scene routes to sacked game over when at_sack_threshold() after results dismiss.
namespace reputation
{

// Named starting value for a new run (title SELECT / boot / campaign-complete reset).
inline constexpr int starting_anger = 0;

// Named sack threshold — G-03 game over when anger reaches this (display uses it too).
inline constexpr int sack_threshold = 100;

// G-02: danger UI when anger is within danger_margin of sack (warning_threshold … sack).
inline constexpr int danger_margin = 20;
inline constexpr int warning_threshold = sack_threshold - danger_margin; // 80

// Fail: anger_fail_base + failed_ticket_count * anger_fail_per_open (then clamp).
// H-04: base 12 (was 15) so early Day 1 fails leave more room to recover.
// Examples: fail 0 open → +12; fail 5 open → +22; fail 10 open → +32.
inline constexpr int anger_fail_base = 12;
inline constexpr int anger_fail_per_open = 2;

// Strong pass relief: completion_percent >= strong_pass_percent → reduce anger.
// Examples: pass 90%+ → -5; pass 75–89% → no change.
inline constexpr int strong_pass_percent = 90;
inline constexpr int anger_strong_pass_relief = 5;

// Current anger (0 … sack_threshold). Higher = closer to sack.
[[nodiscard]] int anger();

// G-02: true when anger >= warning_threshold (readable before sack).
[[nodiscard]] bool near_sack();

// True when anger is at the sack line — shift_scene shows sacked after results dismiss.
[[nodiscard]] bool at_sack_threshold();

// New run: back to starting_anger.
void reset();

// Apply one finished shift after Phase B evaluate (75% gate).
// Fail worsens anger; strong pass improves slightly; normal pass is neutral.
void apply_shift_outcome(int completion_percent, int failed_count, bool passed);

}

#endif
