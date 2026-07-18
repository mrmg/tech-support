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

- [ ] ROM builds with Butano and runs in mGBA
- [ ] Player walks office with depth (up/down/left/right) and desk collision
- [ ] Tickets appear over time (not all at t=0); shift timer counts down
- [ ] Notepad (Select) lists active tickets; timer/urgency pause while open
- [ ] Open tickets have world feedback (PC state and off-screen icon cue)
- [ ] Hold-A reboot at a desk clears that ticket; release/leave resets progress
- [ ] Urgency can increase without failing or removing tickets mid-shift
- [ ] Shift end shows fixed vs still-open counts; can restart

## Tickets

### - [ ] A-01 — Butano project scaffold

- **Done when:** `make` produces a runnable `.gba` (hello sprite or blank scene) in mGBA; README notes toolchain setup and `LIBBUTANO` path.
- **Notes:** Clean Butano template; do not fork Sips. Do this in a normal chat, not `/loop`.

### - [ ] A-02 — Title scene stub

- **Done when:** Boot shows a title (placeholder art OK); pressing **A** (or Start as secondary) enters the shift scene.
- **Notes:** Text-only OK. Prefer **A** as primary confirm so Start stays reserved.

### - [ ] A-03 — Office background + camera

- **Done when:** Shift scene shows a placeholder office BG larger than 240×160; camera follows player within bounds.
- **Notes:** Match 3/4 depth feel; proportions must allow up/down movement lanes.

### - [ ] A-04 — Player movement + collision

- **Done when:** D-pad moves player in four directions; cannot walk through desk/wall collision boxes.
- **Notes:** Tunable speed constant.

### - [ ] A-05 — Desk entities (×4)

- **Done when:** Four desks exist with positions, interact ranges, and idle vs broken visual states (placeholder frames OK).
- **Notes:** Data-driven desk table preferred over hardcoding in `main`.

### - [ ] A-06 — Shift timer HUD

- **Done when:** Configurable shift length counts down on HUD; hits zero ends the shift. Default constant **120** seconds.
- **Notes:** Named constant for easy tweaking. Use “shift” in code/comments, not “day.”

### - [ ] A-07 — Ticket model + spawner

- **Done when:** Tickets spawn on a curve during the shift, bind to free desks, appear in game state (UI not required yet).
- **Notes:** No tickets at t=0; first spawn ~5s suggested; spawn params tweakable.

### - [ ] A-08 — Notepad UI

- **Done when:** **Select** toggles notepad overlay listing active tickets (desk/person, issue line, urgency). While open, shift timer and urgency pause; player cannot move/fix.
- **Notes:** Visual nod to `docs/references/sips-notepad-style.png`.

### - [ ] A-09 — World cues

- **Done when:** Broken desks show a clear PC state; off-screen tickets get an edge indicator with a **simple issue icon** toward their desk.
- **Notes:** No coworker-face portraits in Phase A.

### - [ ] A-10 — Hold-to-reboot

- **Done when:** In interact range + hold **A** fills a progress bar and clears the ticket; releasing A **or** leaving range **resets progress to zero**; PC returns to idle when cleared.
- **Notes:** Phase A reboot type only. This is hold-to-fix, not a start-then-channel-away system.

### - [ ] A-11 — Soft urgency

- **Done when:** Open tickets increase urgency over time; notepad and/or cues reflect it; tickets remain fixable; no game over and no mid-shift ticket failure/removal.
- **Notes:** Prepares Phase B end-of-shift ✗ accounting (failures are assigned at the bell in B, not here).

### - [ ] A-12 — Shift end + retry

- **Done when:** On timer end, show fixed count vs still-open count; **A** returns to title or restarts shift.
- **Notes:** No ✓/✗ list and no 75% gate — Phase B. Still-open ≠ failed yet.

### - [ ] A-13 — Phase A pass (integration)

- **Done when:** All acceptance criteria above are verified in mGBA; tickets A-01…A-12 are `- [x]`; README has build & run notes.
- **Notes:** Human verification OK; list known jank for Phase B under Notes here when done.
