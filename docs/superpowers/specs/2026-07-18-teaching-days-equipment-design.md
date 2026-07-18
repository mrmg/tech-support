# Teaching days and equipment-specific support

**Date:** 2026-07-18
**Status:** Implemented (Phase J complete)

## Problem

Phase I makes failures readable and less predictable, but every issue is still generated from one common pool and attached to a computer desk. That weakens the fiction and asks the player to understand reboot, toner, PSU, and server work immediately.

The campaign should teach one support concept per short day. A toner fault belongs to a printer, a PSU fault belongs to a computer, and a server outage is a single office-wide event. Supplies should be legible physical objects: the fault, its storage bin, and the carried item must share one icon.

## Goals

1. **One concept per day** — Days 1–4 introduce reboot, toner, PSU, then server recovery.
2. **Short, readable shifts** — each day is long enough to teach its verb without becoming repetitive.
3. **Target-specific incidents** — computer, printer, and server faults behave differently.
4. **Weighted, controlled variety** — reboot is common; toner is less common; PSU is rare; server is a scripted global event.
5. **No tutorial soft-locks** — the first toner and PSU incidents always have one matching item available.
6. **One visual language** — a fault icon exactly matches its supply-bin and carried-item icon.
7. **Optional energy tradeoff** — sprinting is useful, exhaustion is recoverable with coffee, and normal play is not forced to use it.

## Non-goals

- Professional final art; clear placeholder sprites are acceptable.
- More than four campaign days in this phase.
- New shop currencies or changes to the pass, anger, or sacking formulas.
- Paper jams, password resets, network cables, or software installs; these remain future incident candidates.
- Multiple simultaneous server incidents.

## Campaign structure

The campaign becomes four deliberately short teaching days. Passing Day 4 completes the campaign and enters the existing credits flow.

| Day | Shift | Guaranteed teaching event | Random incident pool after teaching event |
|---|---:|---|---|
| 1 | 45s | First computer needs reboot | reboot 100% |
| 2 | 55s | One printer needs toner | reboot 70%, printer toner 30% |
| 3 | 60s | One computer needs a PSU | reboot 70%, printer toner 20%, PSU 10% |
| 4 | 60s | One global server outage | reboot 70%, printer toner 20%, PSU 10%; server is scripted separately |

The guaranteed event is deterministic in *kind*, but its eligible target may be selected randomly. After it has appeared, later incidents use the day's weighted pool. Spawn timing remains tunable independently from incident weights.

Day-specific pools are hard gates: toner cannot appear on Day 1, PSU cannot appear before Day 3, and the global outage cannot appear before Day 4.

## Incident targets

### Computer desks

- Reboot remains the fastest and most common incident.
- PSU is a rare computer-only incident and consumes one carried PSU.
- A toner icon must never appear above a computer.
- During a global server outage, every computer shows the same network/server error state rather than receiving separate tickets.

### Printers

- Add two distinct printer entities, one toward each end of the office floor.
- Toner incidents select a free printer, not a computer desk.
- A broken printer shows the toner icon used by the toner bin and carried cartridge.
- Holding A at the printer with toner completes the incident and consumes one toner.
- Printers participate in collision, camera visibility, notepad history, results, urgency, and edge cues.

### Global server outage

- The outage is one incident with one outcome, not one ticket per computer.
- All computer screens visibly enter a shared server/network error state.
- Normal computer incidents stop spawning while the outage is active. Existing non-server work may remain open, but the timer continues.
- The world cue points to the server-room door, then to the rack after entering the server room.
- Holding A at the rack restores the office, clears the shared error state, and records one fixed server incident.
- Only one outage is scheduled on Day 4. It must not spawn while another outage is active.

## Supply area and icon contract

Replace the current single cupboard interaction that cycles toner and PSU with two adjacent, separately interactable supply bins:

| Bin | Matching fault target | Shared icon | Day introduced |
|---|---|---|---:|
| Toner bin | Printer | toner cartridge | 2 |
| PSU bin | Computer | PSU | 3 |

Hard rule: **fault icon → supply-bin icon → carried icon must use the same glyph/frame.** The player should never need text or trial-and-error to match a part.

- Each bin displays its current count or a clear empty/`0` state.
- A at a bin collects that exact part when stock is available.
- If carrying the other part, A swaps only when the requested bin has stock; otherwise show the existing denial feedback.
- B while in range returns the carried part to its matching bin.
- Highlight only the bin currently in interaction range.
- Remove the hidden A-to-cycle behaviour from the old cupboard.

## Stock and tutorial deliveries

- A new campaign starts with toner `0` and PSU `0`.
- At the start of Day 2, deliver one tutorial toner cartridge into the toner bin exactly once per campaign.
- At the start of Day 3, deliver one tutorial PSU into the PSU bin exactly once per campaign.
- Tutorial deliveries must also occur on a retry if the player consumed or lost the required item before the guaranteed incident can be completed; do not allow an unwinnable teaching shift.
- Shop purchases continue to queue and arrive in the matching bin. Tutorial deliveries are free and do not affect budget.
- Later same-day incidents require remaining/purchased stock. An empty bin is a visible resource failure, not a hidden interaction failure.

## Coffee and fatigue

Coffee is an optional routing decision, not a mandatory maintenance timer.

- Add a coffee station on the office floor.
- R is sprint: faster than normal Phase I movement while held.
- Sprinting drains an energy/fatigue meter. Ordinary movement does not drain it.
- At zero energy, movement falls to half normal speed until the player drinks coffee.
- Hold A at the coffee station for a short brew interaction to refill energy.
- Energy starts full at the beginning of every shift, including retries. This prevents one bad shift from poisoning the next.
- The HUD may be a small bar/icon; warnings should become prominent only near exhaustion.
- Coffee and sprinting pause while the notepad is open and must respect collisions and room transitions.

## UI, results, and persistence

- The notepad names targets clearly: `PC reboot`, `Printer toner`, `PC PSU`, `Server outage`.
- Results count a global outage as one incident.
- Edge cues reuse the same incident icon language and route through the correct room door.
- Day intros include a one-line lesson on the newly introduced concept.
- Title/new game, campaign completion, and sacking reset tutorial-delivery and fatigue state consistently with existing campaign state.
- Session-only persistence remains unchanged; no SRAM is added.

## Success feel

1. Day 1 is immediately understandable: find a broken computer and reboot it.
2. Day 2 points to a printer; its toner symbol matches the visible toner bin and carried cartridge.
3. Day 3 introduces a rarer PSU replacement without making it impossible for lack of stock.
4. Day 4 makes the whole office visibly fail at once and sends the player to the server room for one high-impact recovery.
5. Sprinting can save a route, but overusing it creates a meaningful coffee stop rather than unavoidable busywork.

## Future incident candidates

- **Paper jam:** printer-only directional interaction with no stock requirement.
- **Software installation:** start an asynchronous install and return when it completes.
- **Password reset:** inspect a computer, use an admin terminal, then return to the user.
- **Network cable:** collect and fit a visually distinct cable at a computer.

These should be considered only after the four teaching days are playable and fun.
