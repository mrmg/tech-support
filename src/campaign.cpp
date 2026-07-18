#include "campaign.h"

namespace campaign
{

namespace
{

int g_current_day = 1;

// Index 0 = Day 1 (Phase A baseline). Later rows: shorter gaps → denser spawns.
constexpr day_difficulty day_table[max_days] = {
    // first, interval, shrink, min, shift_duration
    { 5, 18, 2, 8, 120 }, // Day 1 — current baseline
    { 4, 15, 2, 7, 120 }, // Day 2
    { 4, 12, 2, 6, 120 }, // Day 3
    { 3, 10, 2, 5, 120 }, // Day 4
    { 3, 8, 1, 4, 120 },  // Day 5
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
