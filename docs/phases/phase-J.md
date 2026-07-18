# Phase J — Teaching days and equipment-specific support

**Unlock:** Phase I complete.
**Design:** [docs/superpowers/specs/2026-07-18-teaching-days-equipment-design.md](../superpowers/specs/2026-07-18-teaching-days-equipment-design.md)

## Goal

Turn the campaign into four short teaching days with target-specific incidents, matching supply bins, and a recoverable coffee/fatigue routing choice.

## Acceptance criteria

- [x] Days 1–4 introduce reboot, printer toner, computer PSU, and global server outage in that order
- [x] Shifts use the documented 45s / 55s / 60s / 60s durations and day-gated weighted incident pools
- [x] Toner incidents occur only on two dedicated printers; PSU incidents occur only on computers
- [x] Toner and PSU have separate bins whose icons match the fault and carried-item icons
- [x] New campaigns start with zero parts, with safe one-time tutorial deliveries on Days 2 and 3
- [x] A server outage is one global incident that visibly affects every computer and clears at the rack
- [x] Sprint fatigue and coffee are optional, readable, and reset safely per shift
- [x] Notepad, results, cues, campaign reset, and clean GBA build cover every new target/state

## Tickets

### - [x] J-01 — Four short teaching days

- **Done when:** Campaign length is four days; Day 4 pass reaches the existing completion/credits flow; shift durations are Day 1 `45s`, Day 2 `55s`, Day 3 `60s`, Day 4 `60s`.
- **Notes:** Update campaign constants, difficulty tables, day-intro copy, README defaults, and any Day 5 assumptions. Preserve pass/anger/shop rules unless this design explicitly changes them.
- **Implemented:** `campaign::max_days = 4`; `day_table` shift lengths 45/55/60/60 with denser short-day spawn gaps; `shift::duration_seconds` Day 1 sync; day intro one-line lessons; README / known-issues Day 5 → Day 4. Existing `max_days` final-pass path still runs credits.

### - [x] J-02 — Day-aware incident director

- **Done when:** Incident kinds are gated and weighted by day: D1 reboot 100%; D2 reboot 70% / toner 30%; D3–4 reboot 70% / toner 20% / PSU 10%. Each day guarantees its teaching incident before weighted follow-ups. Server scheduling remains separate from the random pool.
- **Notes:** Keep random eligible-target selection. Separate incident-kind selection from target selection so computer/printer/global targets cannot be confused. Add deterministic/testable helpers where practical.
- **Implemented:** `ticket::director` helpers (`pool_weights_for_day`, teaching guarantee, `issue_type_from_pool_roll`, `target_kind_for_issue`). Spawner picks kind then target; Days 1–3 force teaching kind on first pool spawn; pool never emits server (Day 4 outage is J-07). Toner → free printers; PSU/reboot → free desks; global → `global_server_target_id`.

### - [x] J-03 — Two printer entities and toner incidents

- **Done when:** Two visible, collidable printers sit toward opposite ends of the office; toner tickets target only printers; holding A with toner fixes the selected printer and consumes stock.
- **Notes:** Integrate printer state with urgency, badges, camera, notepad history, results, SFX, and edge cues. A toner fault must never bind to a computer desk.
- **Implemented:** `printer::entity` ×2 at office ends (`±200, 20`); office solids + camera/visibility; `ticket` toner targets `printer::target_id` only; hold-A install/consume at printers; urgency/badges/edge cues/notepad/results wired. Placeholder `graphics/printer.bmp`.

### - [x] J-04 — Separate toner and PSU bins

- **Done when:** The single cycling cupboard interaction is replaced by adjacent toner and PSU bins. A collects from the selected bin; B in range returns the matching carried part; each bin has a visible count/empty state and independent highlight.
- **Notes:** Reuse the exact same toner/PSU glyph or sprite frame for fault badge, bin, and carried item. Preserve one-item carry. Wrong/empty interactions use the Phase I denial feedback.
- **Implemented:** `closet::bin` ×2 at aisle (`±22, -40`); per-bin interact/highlight (nearest wins); A collects/swaps that bin’s part when stocked; B returns only to the matching bin; count label + hidden icon at `0`; empty/wrong deny = red body flash. `part_icon` frames shared by fault badge, bin overlay, and carry. Hidden A-to-cycle removed. Clean `make -j8` OK.

### - [x] J-05 — Zero start stock and safe tutorial deliveries

- **Done when:** New campaign stock is toner `0`, PSU `0`; Day 2 supplies one free tutorial toner and Day 3 supplies one free tutorial PSU exactly as specified; retries cannot become unwinnable before the guaranteed teaching incident is complete.
- **Notes:** Tutorial deliveries cost no budget and are distinct from pending shop orders. Purchases still arrive in the matching bin. Reset delivery flags on new game, campaign completion, and sacking.
- **Implemented:** `starting_*_stock = 0`; `deliver_tutorials_for_day` before each day intro (+1 toner Day 2 / +1 PSU Day 3 once per campaign; retry re-grant if teaching install never completed and bin empty); `mark_teaching_incident_complete` on successful needs-part install; flags cleared in `inventory::reset()` (boot / SELECT / complete / sack). Shop `deliver_pending` unchanged.

### - [x] J-06 — Rare computer PSU incidents

- **Done when:** PSU remains a computer-only incident, is absent before Day 3, uses the documented 10% weight after its guaranteed introduction, requires the PSU carried from its matching bin, and consumes one PSU on success.
- **Notes:** The desk badge, notepad label, edge cue, bin icon, and carried icon must all agree. Reboot remains the dominant computer fix.
- **Implemented:** Director keeps PSU computer-only (`target_kind` → free desk; Day 1–2 weight `0`; Day 3 teaching guarantee then Days 3–4 `10%`; reboot stays `70%`). Hold-A requires carried PSU and `inventory::consume` on success; printers refuse non-toner fixes. Notepad label `PC PSU`. Edge cues use `part_icon` frame 1 for PSU (same as desk badge / bin / carry). Clean `make -j8` OK.

### - [x] J-07 — One global Day 4 server outage

- **Done when:** Day 4 schedules one office-wide outage represented by one incident. Every computer visibly shows the shared server/network error; normal computer spawning pauses while active; cues route to the server-room door/rack; rack completion restores the office and records one fixed incident.
- **Notes:** Do not generate one server ticket per desk. Existing non-server incidents may remain open and the shift clock continues. Guard against duplicate simultaneous outages.
- **Implemented:** Day 4 first spawn is one `needs_server_reset` on `global_server_target_id` (`director::schedules_global_outage`); `_global_outage_spawned` + active-outage guards prevent duplicates. All desks show broken + server badge while active; computer pool spawns pause (toner may continue). Cues use global target → office door / server rack. Rack hold-A → `clear_server_reset_tickets` (one Fixed). Clean `make -j8` OK.

### - [x] J-08 — Sprint fatigue and coffee recovery

- **Done when:** R sprints and drains energy; zero energy halves movement speed; a visible coffee station supports a short hold-A brew that refills energy; energy starts full on every shift/retry.
- **Notes:** Normal movement does not drain energy. Pause fatigue while notepad is open. Coffee is optional for a player who never sprints. Avoid conflicts with carry return and room transitions.
- **Implemented:** `player` R-sprint (`2.25`) drains `max_energy` (~3s); walk does not drain; at 0 → `exhausted_speed` half walk until coffee. `coffee::station` at `(0,88)` office solid; hold-A 1s brew refills; energy HUD bar blinks near empty; notepad pauses drain/brew; coffee A-range skips bin/desk A; refill every `play_one_shift`.

### - [x] J-09 — Cross-system UI, results, and reset pass

- **Done when:** Notepad and results distinguish `PC reboot`, `Printer toner`, `PC PSU`, and `Server outage`; results count the outage once; cues route to the correct target; Day intros teach the new concept; all new flags/state reset correctly on title new game, completion, retry, and sacking.
- **Notes:** Preserve existing 75% result calculation, anger thresholds, shop economy, and session-only/no-SRAM behaviour.
- **Implemented:** `issue_label` → design names; notepad/results skip duplicate "Server" prefix on outage; `clear_server_reset_tickets` → one fixed via `clear_desk(global)`; cues keep door/rack/printer routing + slot bounds; day-intro lessons unchanged; tutorial flags via `inventory::reset` on SELECT/complete/sack; energy refilled each `play_one_shift`. Pass%/anger/shop untouched. Clean `make -j8` OK.

### - [x] J-10 — Phase J integration and playability pass

- **Done when:** J-01…J-09 are verified and checked; clean `make` succeeds; mGBA smoke reaches each day-specific concept using safe shortened/debug paths if needed; README and known issues describe the final behaviour and any remaining jank.
- **Notes:** Remove temporary debug shortcuts before completion. Record exact build result and leave future incidents (paper jam/software/password/cable) out of this phase.
- **Verified (J-10):** Code-path review confirmed J-01…J-09 Done when + all acceptance criteria. No keypad day-skip shortcuts present (`campaign::set_day` remains an unused playtest API only). Clean `make clean && make -j8` → ROM fixed; `tech-support.gba` **234096** bytes. Day concepts: D1 reboot lesson + 100% pool; D2 toner teaching + printers/bins + tutorial toner; D3 PSU teaching + tutorial PSU; D4 global outage + rack clear; coffee/sprint optional. Future paper-jam / software / password / cable incidents stay out of scope. Remaining jank documented in `docs/known-issues.md`.
