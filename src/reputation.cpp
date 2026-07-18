#include "reputation.h"

namespace reputation
{

namespace
{

int g_anger = starting_anger;

void set_anger_clamped(int value)
{
    if(value < 0)
    {
        value = 0;
    }

    if(value > sack_threshold)
    {
        value = sack_threshold;
    }

    g_anger = value;
}

}

int anger()
{
    return g_anger;
}

bool near_sack()
{
    return g_anger >= warning_threshold;
}

bool at_sack_threshold()
{
    return g_anger >= sack_threshold;
}

void reset()
{
    g_anger = starting_anger;
}

void apply_shift_outcome(int completion_percent, int failed_count, bool passed)
{
    if(completion_percent < 0)
    {
        completion_percent = 0;
    }

    if(completion_percent > 100)
    {
        completion_percent = 100;
    }

    if(failed_count < 0)
    {
        failed_count = 0;
    }

    if(! passed)
    {
        set_anger_clamped(g_anger + anger_fail_base + failed_count * anger_fail_per_open);
        return;
    }

    if(completion_percent >= strong_pass_percent)
    {
        set_anger_clamped(g_anger - anger_strong_pass_relief);
    }
}

}
