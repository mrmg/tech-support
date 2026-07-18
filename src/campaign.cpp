#include "campaign.h"

namespace campaign
{

namespace
{

int g_current_day = 1;

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

}
