#ifndef INVENTORY_H
#define INVENTORY_H

#include "carry.h"

// Session IT budget + closet part stock (Phase F). Session memory only — no SRAM.
// F-01: track budget/stock; closet pickup gated on stock; desk install consumes stock.
// F-02: earn budget from shift results (see budget_earn_for_shift).
// F-03: between-day shop spends budget via try_buy (see shop prices).
// F-04 delivery rule: shop purchases do NOT add closet stock immediately.
//   try_buy queues +1 pending for that part (budget deducted now).
//   deliver_pending() moves pending → stock when the campaign advances to the
//   next day (after shop, before Day N+1 intro/shift). Closet uses stock only.
namespace inventory
{

// Named starting values for a new run (title SELECT / boot / campaign-complete reset).
// H-04: +1 toner so Day 1 parts pressure is fair before the first shop.
inline constexpr int starting_budget = 100;
inline constexpr int starting_toner_stock = 4;
inline constexpr int starting_psu_stock = 2;

// F-02 earn constants (integer arithmetic; truncates toward zero).
// Pass: pass_earn_base + completion_percent / pass_earn_divisor
// Fail: completion_percent / fail_earn_divisor
// Examples: pass 75% → 35; pass 100% → 40; fail 50% → 5; fail 74% → 7.
inline constexpr int pass_earn_base = 20;
inline constexpr int pass_earn_divisor = 5;
inline constexpr int fail_earn_divisor = 10;

// F-03 shop prices (budget units per +1 pending order → next-day stock).
inline constexpr int toner_price = 15;
inline constexpr int psu_price = 25;

[[nodiscard]] int budget();
[[nodiscard]] int stock_of(carry::part part);

// Units ordered in shop but not yet delivered (available next day).
[[nodiscard]] int pending_of(carry::part part);

// Shop catalog price for toner/psu; 0 for none/unknown.
[[nodiscard]] int price_of(carry::part part);

// True when the closet can hand out this part type (stock > 0).
[[nodiscard]] bool has_stock(carry::part part);

// Desk install path: permanently spend one unit. Returns false if none/zero stock.
bool consume(carry::part part);

// New run: restore starting budget + stock; clear pending orders.
void reset();

void set_budget(int value);
void add_budget(int delta);

// Closet stock change (order delivery / debug). Clamps at 0.
void add_stock(carry::part part, int delta);

// F-03/F-04: spend price_of(part) and queue +1 pending when affordable.
// False if none/can't afford. Stock increases only via deliver_pending().
bool try_buy(carry::part part);

// F-04: apply all pending orders to closet stock, then clear pending.
// Call once when the campaign advances to the next day (not on fail / same-day retry).
void deliver_pending();

// F-02: grant amount for one finished shift after Phase B evaluate (75% gate).
// Pass always grants more than fail (fail % is always below the pass threshold).
[[nodiscard]] int budget_earn_for_shift(int completion_percent, bool passed);

}

#endif
