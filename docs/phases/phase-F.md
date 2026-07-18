# Phase F — Stock & budget

**Unlock:** Phase D complete (parts exist). Phase E complete — prefer F next.  
**Uses Phase C between-day moments.**  
**Complete.**

## Goal

Between days: spend IT budget to order parts (toner, PSU, …). Stock depletes when used; zero stock blocks matching needs-part fixes until the order arrives next day.

## In scope

- Budget currency from shift performance (e.g. based on completion %)
- Stock counts for at least two part types
- Between-day shop UI (after pass, before next day intro — or on fail skip shop)
- Orders arrive next day (clear rule)

## Out of scope

- Deep accounting sim
- Phase G sacking

## Acceptance criteria

- [x] Player can spend budget on at least two item types
- [x] Zero stock blocks the matching fix until ordered
- [x] Orders arrive on a clear rule (next day)

## Tickets

### - [x] F-01 — Budget + stock model

- **Done when:** Session tracks budget and stock for ≥2 part types; using a part in D consumes stock; closet pickup fails or is blocked at zero stock.
- **Notes:** Named starting stock/budget constants (`inventory::starting_*`). Session module `inventory`; closet A gated on `has_stock`; desk install calls `consume`. Shop/earn out of scope (F-02/F-03).
- **mGBA:** Exhaust stock → cannot pick up that part.

### - [x] F-02 — Earn budget from shift results

- **Done when:** Passing/failing a day adjusts budget (pass grants more; formula documented). 
- **Notes:** Simple integer formula after Phase B `evaluate` (75% gate), in `apply_shift_budget_earn` via `inventory::add_budget(budget_earn_for_shift(...))`.
  - **Pass:** `pass_earn_base + completion_percent / pass_earn_divisor` → `20 + percent / 5` (75% → 35; 100% → 40).
  - **Fail:** `completion_percent / fail_earn_divisor` → `percent / 10` (50% → 5; 74% → 7).
  - Final-day pass still earns then `inventory::reset()` wipes to `starting_budget` (fresh run).
- **mGBA:** After a pass, budget increased (debug or shop shows it).

### - [x] F-03 — Between-day shop UI

- **Done when:** After a **pass** (before next day intro), shop screen lets player buy ≥2 item types with budget; A/B or D-pad to purchase/leave.
- **Notes:** Skip shop on fail (retry same day), title exit, and final-day pass (reset wipes stock). Notepad panel + ink. Catalog: toner `$15`, PSU `$25`. Controls: Up/Down select, A buy, B leave. Buys via `inventory::try_buy` (F-04: queues pending, not immediate stock).
- **mGBA:** Pass → shop → buy toner → leave → Day N+1.

### - [x] F-04 — Orders arrive next day

- **Done when:** Purchases increase stock available for the **next** day (prefer **next day** per design). Pending orders visible in shop/HUD.
- **Notes / delivery rule:** `try_buy` deducts budget and queues `pending` (+1 per purchase). Closet still reads `stock` only. After a mid-campaign pass, `apply_shift_campaign_progress` calls `campaign::advance()` then `inventory::deliver_pending()` so Day N orders become stock for Day N+1 intro/shift. Shop rows show `xN` stock and `+N` pending; footer `(+N next day)`. `reset()` clears pending. Fail skips shop (no new orders); final-day pass skips shop and `reset()` wipes economy.
- **mGBA:** Buy when stock 0 → next day can pick up part.

### - [x] F-05 — Phase F pass (integration)

- **Done when:** Acceptance criteria met; F-01…F-04 `- [x]`; README updated; jank for G listed.
- **Notes:**
  - **Verification:** F-01…F-04 already `- [x]`. Acceptance criteria confirmed by code-path review: `inventory` session budget + toner/PSU `stock`/`pending`; closet A gated on `has_stock`; desk needs-part install calls `consume`; `apply_shift_budget_earn` after results (`20 + %/5` pass, `%/10` fail); mid-campaign pass + continue → `run_shop_scene` (toner `$15` / PSU `$25`, Up/Down/A/B) via `try_buy` → pending; then `campaign::advance` + `deliver_pending` before Day N+1 intro; fail/title/final-pass skip shop; final-pass `inventory::reset()`.
  - **Build:** `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba`. Pass a day → shop → buy when stock low → leave → next day closet has delivery; exhaust a part → pickup blocked until order arrives.
  - **Known jank for Phase G:** zero-stock closet A and can't-afford shop A are **silent** (no deny flash/SFX); pass→title skips shop (earn applies, day still advances, no between-day spend); no in-shift budget/stock HUD (pending only on shop rows); carry pickup does not reserve stock until install (last unit still counts as on-hand while held); final-day pass earn wiped by `reset`; A overloaded further (shop buy + closet/desk/rack/confirms); Phase E leftovers remain (rack hold with no server ticket, one rack clear fixes all open server-resets, door cue stacking, instant room swap, silent wrong/missing part, placeholder art); Phase C leftovers remain (120s days, no SRAM, bare day intro, OK/X results, 4-row cap, A/SELECT discoverability).
- **mGBA:** Full economy loop across two days.
