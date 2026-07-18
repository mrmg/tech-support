#include "inventory.h"

namespace inventory
{

namespace
{

int g_budget = starting_budget;
int g_toner_stock = starting_toner_stock;
int g_psu_stock = starting_psu_stock;
int g_toner_pending = 0;
int g_psu_pending = 0;

// J-05 tutorial grants (session only; cleared by reset with campaign/new game/sack).
bool g_tutorial_toner_delivered = false;
bool g_tutorial_psu_delivered = false;
bool g_toner_teaching_complete = false;
bool g_psu_teaching_complete = false;

[[nodiscard]] int& stock_ref(carry::part part)
{
    // toner/psu only; caller must not pass none.
    if(part == carry::part::psu)
    {
        return g_psu_stock;
    }

    return g_toner_stock;
}

[[nodiscard]] int& pending_ref(carry::part part)
{
    if(part == carry::part::psu)
    {
        return g_psu_pending;
    }

    return g_toner_pending;
}

}

int budget()
{
    return g_budget;
}

int stock_of(carry::part part)
{
    switch(part)
    {
    case carry::part::toner:
        return g_toner_stock;

    case carry::part::psu:
        return g_psu_stock;

    case carry::part::none:
    default:
        return 0;
    }
}

int pending_of(carry::part part)
{
    switch(part)
    {
    case carry::part::toner:
        return g_toner_pending;

    case carry::part::psu:
        return g_psu_pending;

    case carry::part::none:
    default:
        return 0;
    }
}

bool has_stock(carry::part part)
{
    return stock_of(part) > 0;
}

bool consume(carry::part part)
{
    if(part == carry::part::none || ! has_stock(part))
    {
        return false;
    }

    --stock_ref(part);
    return true;
}

int price_of(carry::part part)
{
    switch(part)
    {
    case carry::part::toner:
        return toner_price;

    case carry::part::psu:
        return psu_price;

    case carry::part::none:
    default:
        return 0;
    }
}

void reset()
{
    g_budget = starting_budget;
    g_toner_stock = starting_toner_stock;
    g_psu_stock = starting_psu_stock;
    g_toner_pending = 0;
    g_psu_pending = 0;
    g_tutorial_toner_delivered = false;
    g_tutorial_psu_delivered = false;
    g_toner_teaching_complete = false;
    g_psu_teaching_complete = false;
}

void set_budget(int value)
{
    if(value < 0)
    {
        value = 0;
    }

    g_budget = value;
}

void add_budget(int delta)
{
    set_budget(g_budget + delta);
}

void add_stock(carry::part part, int delta)
{
    if(part == carry::part::none || delta == 0)
    {
        return;
    }

    int& stock = stock_ref(part);
    stock += delta;

    if(stock < 0)
    {
        stock = 0;
    }
}

bool try_buy(carry::part part)
{
    const int price = price_of(part);

    if(price <= 0 || g_budget < price)
    {
        return false;
    }

    // F-04: deduct budget now; stock arrives next day via deliver_pending().
    set_budget(g_budget - price);
    ++pending_ref(part);
    return true;
}

void deliver_pending()
{
    // Move queued shop orders into closet stock (toner then PSU).
    if(g_toner_pending > 0)
    {
        add_stock(carry::part::toner, g_toner_pending);
        g_toner_pending = 0;
    }

    if(g_psu_pending > 0)
    {
        add_stock(carry::part::psu, g_psu_pending);
        g_psu_pending = 0;
    }
}

void deliver_tutorials_for_day(int day)
{
    // Day 2: one free toner once per campaign; retry again if teaching never completed
    // and the bin is empty (consumed/lost before the guaranteed install).
    if(day == 2 && ! g_toner_teaching_complete)
    {
        if(! g_tutorial_toner_delivered)
        {
            add_stock(carry::part::toner, 1);
            g_tutorial_toner_delivered = true;
        }
        else if(stock_of(carry::part::toner) == 0)
        {
            add_stock(carry::part::toner, 1);
        }
    }

    // Day 3: same rule for PSU.
    if(day == 3 && ! g_psu_teaching_complete)
    {
        if(! g_tutorial_psu_delivered)
        {
            add_stock(carry::part::psu, 1);
            g_tutorial_psu_delivered = true;
        }
        else if(stock_of(carry::part::psu) == 0)
        {
            add_stock(carry::part::psu, 1);
        }
    }
}

void mark_teaching_incident_complete(carry::part part)
{
    switch(part)
    {
    case carry::part::toner:
        g_toner_teaching_complete = true;
        break;

    case carry::part::psu:
        g_psu_teaching_complete = true;
        break;

    case carry::part::none:
    default:
        break;
    }
}

int budget_earn_for_shift(int completion_percent, bool passed)
{
    if(completion_percent < 0)
    {
        completion_percent = 0;
    }

    if(completion_percent > 100)
    {
        completion_percent = 100;
    }

    // Integer formula (documented in inventory.h + phase-F F-02 notes):
    //   pass → pass_earn_base + completion_percent / pass_earn_divisor
    //   fail → completion_percent / fail_earn_divisor
    if(passed)
    {
        return pass_earn_base + completion_percent / pass_earn_divisor;
    }

    return completion_percent / fail_earn_divisor;
}

}
