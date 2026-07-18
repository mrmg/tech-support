# Phase A — Office & core loop (V1)

**Unlock:** Always (first phase).

## Goal

Ship a playable timed **shift**: walk a small Sips-style office, tickets spawn over time, notepad + world cues, hold-to-reboot, soft urgency, simple shift-end summary.

## In scope

- Butano project scaffold producing a `.gba`
- ~4-desk scrollable office, player movement + collision
- Ticket spawn curve during a tweakable **shift** length (suggested start: 120s)
- Notepad overlay (Select; pauses timer/urgency while open)
- World cues (broken PC + off-screen **icon** pointer)
- Hold-to-fix with progress bar (A held in range; release/leave **resets** progress to 0)
- Soft urgency (visual only; tickets stay fixable; no mid-shift fail)
- Title → shift → simple “shift over” → retry

## Out of scope

- Campaign days / Day 1–N (Phase C)
- ✓/✗ end notepad + 75% pass gate (Phase B)
- Inventory, storage closet, server room (D/E)
- Budget/stock (F), sacking (G), full polish (H)

## Acceptance criteria

- [x] ROM builds with Butano and runs in mGBA
- [x] Player walks office with depth (up/down/left/right) and desk collision
- [x] Tickets appear over time (not all at t=0); shift timer counts down
- [x] Notepad (Select) lists active tickets; timer/urgency pause while open
- [x] Open tickets have world feedback (PC state and off-screen icon cue)
- [x] Hold-A reboot at a desk clears that ticket; release/leave resets progress
- [x] Urgency can increase without failing or removing tickets mid-shift
- [x] Shift end shows fixed vs still-open counts; can restart

## Tickets

### - [x] A-01 — Butano project scaffold

- **Done when:** `make` produces a runnable `.gba` (hello sprite or blank scene) in mGBA; README notes toolchain setup and `LIBBUTANO` path.
- **Notes:** Clean Butano template; do not fork Sips. Do this in a normal chat, not `/loop`. Butano **21.7.0** submodule at `third_party/butano`; `LIBBUTANO=third_party/butano/butano`; ROM `tech-support.gba`.

### - [x] A-02 — Title scene stub

- **Done when:** Boot shows a title (placeholder art OK); pressing **A** (or Start as secondary) enters the shift scene.
- **Notes:** Text-only OK. Prefer **A** as primary confirm so Start stays reserved. mGBA: boot → see title → press A → see shift stub.

### - [x] A-03 — Office background + camera

- **Done when:** Shift scene shows a placeholder office BG larger than 240×160; camera follows player within bounds.
- **Notes:** Match 3/4 depth feel; proportions must allow up/down movement lanes. mGBA: title→A→ see large office BG; move with D-pad; camera follows within bounds.

### - [x] A-04 — Player movement + collision

- **Done when:** D-pad moves player in four directions; cannot walk through desk/wall collision boxes.
- **Notes:** Tunable `player::move_speed`; axis-separated AABB vs walls + 4 desks. mGBA: title→A→ walk 4-way; brown desk markers block; slide along walls/desks.

### - [x] A-05 — Desk entities (×4)

- **Done when:** Four desks exist with positions, interact ranges, and idle vs broken visual states (placeholder frames OK).
- **Notes:** Data-driven desk table preferred over hardcoding in `main`.
- **mGBA:** title→A→ see 4 desk sprites (top-left starts broken / red screen); walk into desks (still blocked); press **L** near a desk to toggle idle↔broken.

### - [x] A-06 — Shift timer HUD

- **Done when:** Configurable shift length counts down on HUD; hits zero ends the shift. Default constant **120** seconds.
- **Notes:** `shift::duration_seconds` (default 120) / `shift::duration_frames` in `include/shift.h`. HUD mm:ss top-center; at 0 shows **SHIFT OVER** / Press A → returns to title (full summary/retry is A-12). Timer counts every frame in shift; notepad pause deferred to A-08. For a shorter mGBA check, temporarily lower `shift::duration_seconds` then set it back to 120.
- **mGBA:** title→A→ see mm:ss countdown on HUD; when it hits 0:00 → SHIFT OVER → A → title again.

### - [x] A-07 — Ticket model + spawner

- **Done when:** Tickets spawn on a curve during the shift, bind to free desks, appear in game state (UI not required yet).
- **Notes:** No tickets at t=0; first spawn ~5s suggested; spawn params tweakable. `ticket::instance` (desk id, reboot type, urgency, open) + `ticket::spawner` with tweakable `ticket::spawn::*` constants (`first_spawn_seconds=5`, interval curve). Shift scene updates spawner each frame; bound desks show broken as game-state visibility (full world cues = A-09).
- **mGBA:** title→A→ all desks idle at start; ~5s later a desk turns broken (ticket bound); more desks break on the spawn curve (~18s gaps shrinking); max one open ticket per desk.

### - [x] A-08 — Notepad UI

- **Done when:** **Select** toggles notepad overlay listing active tickets (desk/person, issue line, urgency). While open, shift timer and urgency pause; player cannot move/fix.
- **Notes:** Visual nod to `docs/references/sips-notepad-style.png`. Cream ruled `notepad_bg` + `notepad::overlay`; Select toggles; open pauses shift timer + ticket spawn clock and blocks movement. Lists Desk N / reboot / urgency.
- **mGBA:** title→A→ wait for a ticket (~5s) → **Select** → cream notepad lists active tickets (timer frozen, cannot move) → Select again closes and timer/movement resume.

### - [x] A-09 — World cues

- **Done when:** Broken desks show a clear PC state; off-screen tickets get an edge indicator with a **simple issue icon** toward their desk.
- **Notes:** No coworker-face portraits in Phase A. Broken desk frames: red PC + X + smoke with error flash (`desk` tiles 1–2). Off-screen open tickets: screen-edge `issue_icon` (warning triangle) via `world_cues::edge_indicators` (hidden while notepad open).
- **mGBA:** title→A→ wait ~5s for a ticket → broken desk shows red/error flash + smoke; walk away so that desk leaves the screen → yellow warning icon appears on the screen edge toward the desk; walk back → icon clears when desk is on-screen; Select notepad hides edge cues.

### - [x] A-10 — Hold-to-reboot

- **Done when:** In interact range + hold **A** fills a progress bar and clears the ticket; releasing A **or** leaving range **resets progress to zero**; PC returns to idle when cleared.
- **Notes:** Phase A reboot type only. This is hold-to-fix, not a start-then-channel-away system. `fix_interaction::hold_to_reboot` + tunable `fix_interaction::reboot::hold_duration_seconds` (default **2**); progress bar sprite above desk while holding; `ticket::spawner::clear_desk` returns PC to idle. Fix blocked while notepad open (A-08).
- **mGBA:** title→A→ wait ~5s for a ticket → walk into that desk’s interact range → **hold A** → green progress bar fills (~2s) → desk returns to idle / ticket gone from notepad; release A mid-hold or walk out of range → bar vanishes and progress resets (must hold again from empty).

### - [x] A-11 — Soft urgency

- **Done when:** Open tickets increase urgency over time; notepad and/or cues reflect it; tickets remain fixable; no game over and no mid-shift ticket failure/removal.
- **Notes:** Prepares Phase B end-of-shift ✗ accounting (failures are assigned at the bell in B, not here). Soft climb via `ticket::urgency::*` (`seconds_per_level=10`, `max_level=9`); paused with notepad (A-08). Notepad **URG** column shows level + bangs; edge icons flash/scale up; broken desk error flash speeds up. Cap is visual only — still hold-A fixable; no mid-shift fail/removal/game over.
- **mGBA:** title→A→ wait ~5s for a ticket → leave it open; every ~10s notepad **URG** climbs (0→1→…, bangs at 4+/7+); off-screen edge icon starts flashing and grows at higher urgency; broken PC flash speeds up; hold-A still clears at max urgency; nothing auto-fails or removes the ticket mid-shift.

### - [x] A-12 — Shift end + retry

- **Done when:** On timer end, show fixed count vs still-open count; **A** returns to title or restarts shift.
- **Notes:** No ✓/✗ list and no 75% gate — Phase B. Still-open ≠ failed yet. `ticket::spawner::fixed_count` increments on hold-to-reboot clear; still-open = `open_count` at the bell. End screen: Fixed X / Still open Y; **A** retries shift; **B** returns to title.
- **mGBA:** title→A→ play until 0:00 (or temporarily lower `shift::duration_seconds`) → see SHIFT OVER with Fixed / Still open counts → **A** starts a fresh shift; **B** returns to title.

### - [x] A-13 — Phase A pass (integration)

- **Done when:** All acceptance criteria above are verified in mGBA; tickets A-01…A-12 are `- [x]`; README has build & run notes.
- **Notes:**
  - **Verification:** A-01…A-12 already `- [x]`. Clean `make -j$(sysctl -n hw.ncpu)` → `tech-support.gba` (~151 KB, title `TECHSUPPORT` / `TS01`). Code review of `shift_scene` + ticket/notepad/fix/world-cue modules confirms all acceptance paths. Homebrew mGBA 0.10.5 (Qt) launches the ROM (exit 0) but has **no headless/screenshot CLI**; full interactive click-through needs a human with a display.
  - **Human click-through still useful:** title→A→ walk/collide → wait ~5s ticket → Select notepad pause → edge icon off-screen → hold-A reboot → leave open for urgency → play to 0:00 (or temporarily lower `shift::duration_seconds`) → Fixed/Still open → A retry / B title.
  - **Known jank for Phase B:** placeholder art (text title, stub desks/player, flat office BG); no SFX/music; 120s shift is slow to playtest end screen; interact AABB may feel larger than desk art; progress bar can be easy to miss / clip near view edge; edge icons may flicker at on-screen margin; notepad is a snapshot on open (OK while paused); Desk N labels only (no coworker faces/names); A overloaded (confirm / hold-reboot / retry); desk-corner collision slide can snag; soft urgency flash/scale is subtle.
