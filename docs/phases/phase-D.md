# Phase D — Carry & parts

**Unlock:** Phase C complete. **Complete.**  
**Does not require Phase E.**

## Goal

One carried item at a time. **Same-map storage closet** (cupboard on the office floor). Ticket types that need a part (toner / PSU) then hold-to-install at the desk. Reboot-only tickets still work.

## In scope

- Carry slot + pickup (drop optional)
- Storage closet zone on the office map
- Multi-step tickets: fetch → install
- Reboot-only tickets unchanged

## Out of scope

- Separate rooms (**Phase E**)
- Ordering stock between days (**Phase F**) — closet has infinite stock for D

## Acceptance criteria

- [x] Player can carry one part
- [x] At least two ticket types: reboot-only and needs-part
- [x] Missing part blocks completion until fetched from the closet
- [x] Closet is reachable without a room transition

## Tickets

### - [x] D-01 — Storage closet on the office map

- **Done when:** A closet/cupboard zone exists on the same 512×256 office map with collision and a clear visual; player can walk up to it.
- **Notes:** Same-map only. Placeholder art OK.
- **mGBA:** See closet; walk to it; blocked by its solid.

### - [x] D-02 — Carry one part

- **Done when:** At closet, press A to pick up a part (toner or PSU); HUD/sprite shows carried item; only one at a time (pickup replaces or refuses if full — pick one rule and document).
- **Notes:** Infinite stock at closet for Phase D. **Pickup rule: replace** — A in closet range: empty → toner; already holding → swap toner ↔ PSU (never two slots). B in closet range returns the part (clears carry). HUD: `part_icon` bottom-right.
- **mGBA:** Pick up part; see carry indicator; cannot hold two different parts at once.

### - [x] D-03 — Needs-part ticket type

- **Done when:** Spawner can create reboot tickets and needs-part tickets (at least toner); notepad shows the required part; desk broken state still used.
- **Notes:** Mix both types during a shift. Data-driven ticket kind enum (`reboot` / `needs_toner` / `needs_psu`). Hold-A on needs-part is blocked until D-04 (no clear without part).
- **mGBA:** See a “needs toner” (or PSU) ticket in notepad.

### - [x] D-04 — Hold-to-install with correct part

- **Done when:** At desk with needs-part ticket, hold A only progresses if carrying the correct part; on complete, consume part, clear ticket, desk idle. Wrong/missing part: no progress (optional short deny feedback). Reboot tickets still use hold-A without a part.
- **Notes:** Reuse progress bar from reboot. Release/leave resets progress. `hold_to_reboot::update` gates needs-* on `carried.held() == required_part`; on complete `clear_desk` + `carried.clear()`.
- **mGBA:** Without part, hold does nothing useful; with part, hold clears ticket and empties carry.

### - [x] D-05 — Phase D pass (integration)

- **Done when:** Acceptance criteria verified; D-01…D-04 `- [x]`; README notes carry/parts; known jank for Phase E listed.
- **Notes:**
  - **Verification:** D-01…D-04 already `- [x]`. Acceptance criteria confirmed by code review: `closet` same-map solid + sprite at aisle center (`office::solid_boxes`); `carry::slot` one part with **replace** A / return B + `part_icon` HUD; `ticket::type` `reboot` / `needs_toner` / `needs_psu` mixed in spawner; notepad `issue_label` shows required part; `hold_to_reboot` gates needs-* on `carried.held() == required_part`, consumes part on clear; reboot hold-A unchanged (no part). Closet pickup skipped while notepad open; A at cupboard before desk hold so pickup wins on press.
  - **Build:** Clean `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make clean && make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba`. No headless/screenshot CLI — interactive click-through needs a human with a display. Optional: shorten day shift length for faster part-ticket cycles, then restore **120**.
  - **Known jank for Phase E:** still **one room** (closet on main floor only — E must add door/transition + second map); wrong/missing part is **silent** (hold resets, no deny flash/SFX); A overloaded further (closet pick/swap + desk hold + title/intro/results confirms) — easy to accidental-swap toner↔PSU at cupboard; B returns part only in closet range (elsewhere unused mid-shift); infinite closet stock (F territory); carry HUD bottom-right may collide with future room UI; placeholder closet/part art; desk interact AABB vs art; progress bar / edge-icon flicker; Phase C leftovers remain (120s days, no SRAM, day intro bare, OK/X results, 4-row cap, A/SELECT discoverability).
- **mGBA:** Reboot still works; needs-part requires closet fetch then install.
