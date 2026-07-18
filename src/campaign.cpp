#include "campaign.h"

namespace campaign
{

namespace
{

int g_current_day = 1;

// J-01 teaching-day defaults. Index 0 = Day 1. Short shifts; later rows denser.
// ≈spawn count is a guide only (desk cap / clears change the real total).
constexpr day_difficulty day_table[max_days] = {
    // first, interval, shrink, min, shift_duration
    { 5, 12, 1, 8, 45 }, // Day 1 — ~4 spawns; reboot teaching
    { 4, 10, 1, 7, 55 }, // Day 2 — ~6; toner teaching
    { 4, 9, 1, 6, 60 },  // Day 3 — ~7–8; PSU teaching
    { 3, 8, 1, 5, 60 },  // Day 4 — densest short finale; server teaching
};

[[nodiscard]] int clamp_day(int day)
{
    if(day < 1)
    {
        return 1;
    }

    if(day > max_days)
    {
        return max_days;
    }

    return day;
}

}

day_difficulty difficulty_for_day(int day)
{
    return day_table[clamp_day(day) - 1];
}

int current_day()
{
    return g_current_day;
}

void reset()
{
    g_current_day = 1;
}

void advance()
{
    if(g_current_day < max_days)
    {
        ++g_current_day;
    }
}

void set_day(int day)
{
    g_current_day = clamp_day(day);
}

}
