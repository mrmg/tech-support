#include "campaign.h"

namespace campaign
{

namespace
{

int g_current_day = 1;

// H-04 playability defaults. Index 0 = Day 1 (matches ticket::spawn::* baseline).
// Later rows: shorter gaps → denser spawns. All shifts 120s. ≈spawn count is a guide only
// (desk cap / clears change the real total).
constexpr day_difficulty day_table[max_days] = {
    // first, interval, shrink, min, shift_duration
    { 6, 20, 2, 10, 120 }, // Day 1 — ~9 spawns; learn day (parts + server room)
    { 5, 16, 2, 8, 120 },  // Day 2 — ~11
    { 4, 13, 2, 7, 120 },  // Day 3 — ~13
    { 3, 11, 2, 6, 120 },  // Day 4 — denser mid-late
    { 3, 9, 1, 5, 120 },   // Day 5 — densest; min gap 5s (not 4) for travel fairness
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
