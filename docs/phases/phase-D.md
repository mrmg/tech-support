# Phase D — Carry & parts

**Unlock:** Phase C complete. **Active.**  
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

- [ ] Player can carry one part
- [ ] At least two ticket types: reboot-only and needs-part
- [ ] Missing part blocks completion until fetched from the closet
- [ ] Closet is reachable without a room transition

## Tickets

### - [x] D-01 — Storage closet on the office map

- **Done when:** A closet/cupboard zone exists on the same 512×256 office map with collision and a clear visual; player can walk up to it.
- **Notes:** Same-map only. Placeholder art OK.
- **mGBA:** See closet; walk to it; blocked by its solid.

### - [ ] D-02 — Carry one part

- **Done when:** At closet, press A to pick up a part (toner or PSU); HUD/sprite shows carried item; only one at a time (pickup replaces or refuses if full — pick one rule and document).
- **Notes:** Infinite stock at closet for Phase D. B or second A at closet can return/swap if useful.
- **mGBA:** Pick up part; see carry indicator; cannot hold two different parts at once.

### - [ ] D-03 — Needs-part ticket type

- **Done when:** Spawner can create reboot tickets and needs-part tickets (at least toner); notepad shows the required part; desk broken state still used.
- **Notes:** Mix both types during a shift. Data-driven ticket kind enum.
- **mGBA:** See a “needs toner” (or PSU) ticket in notepad.

### - [ ] D-04 — Hold-to-install with correct part

- **Done when:** At desk with needs-part ticket, hold A only progresses if carrying the correct part; on complete, consume part, clear ticket, desk idle. Wrong/missing part: no progress (optional short deny feedback). Reboot tickets still use hold-A without a part.
- **Notes:** Reuse progress bar from reboot. Release/leave resets progress.
- **mGBA:** Without part, hold does nothing useful; with part, hold clears ticket and empties carry.

### - [ ] D-05 — Phase D pass (integration)

- **Done when:** Acceptance criteria verified; D-01…D-04 `- [x]`; README notes carry/parts; known jank for Phase E listed.
- **Notes:** Clean make + code review; human mGBA path.
- **mGBA:** Reboot still works; needs-part requires closet fetch then install.
