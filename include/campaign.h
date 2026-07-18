#ifndef CAMPAIGN_H
#define CAMPAIGN_H

// Campaign day state (Phase C). Day = campaign node; Shift = timed play for that day.
// Session memory only — no SRAM save required.
namespace campaign
{

// Finite early campaign length. Day index is 1…max_days.
inline constexpr int max_days = 5;

// Current campaign day (1-based). Starts at 1 after reset / boot.
[[nodiscard]] int current_day();

// New run: back to Day 1.
void reset();

// Pass path (C-04): move to the next day. No-op when already on max_days.
void advance();

}

#endif
