# Phase A — Office & core loop (V1)

## Goal

Ship a playable timed shift: walk a small Sips-style office, tickets spawn over time, notepad + world cues, hold-to-reboot, soft urgency, simple shift-end summary.

## In scope

- Butano project scaffold producing a `.gba`
- ~4-desk scrollable office, player movement + collision
- Ticket spawn curve during a tweakable shift length
- Notepad overlay listing active tickets
- World cues (broken PC + off-screen pointer)
- Hold-to-fix with progress bar
- Soft urgency (visual only; no sacking)
- Title → shift → simple “shift over” → retry

## Out of scope

- Campaign days / Day 1–N (Phase C)
- ✓/✗ end notepad + 75% pass gate (Phase B)
- Inventory, storage, server room (D/E)
- Budget/stock (F), sacking (G), full polish (H)

## Acceptance criteria

- [ ] ROM builds with Butano and runs in mGBA
- [ ] Player walks office with depth (up/down/left/right) and desk collision
- [ ] Tickets appear over time (not all at t=0); shift timer counts down
- [ ] Notepad lists active tickets anytime
- [ ] Open tickets have world feedback (PC state and/or off-screen cue)
- [ ] Hold-to-reboot at a desk clears that ticket
- [ ] Urgency can increase without ending the run mid-shift
- [ ] Shift end shows fixed vs remaining counts; can restart

## Tickets

### A-01 — Butano project scaffold

- **Done when:** `make` produces a runnable `.gba` (e.g. hello sprite or blank scene) in mGBA; README notes toolchain setup.
- **Notes:** Clean Butano template; do not fork Sips. Document `LIBBUTANO` path.

### A-02 — Title scene stub

- **Done when:** Boot shows a title (placeholder art OK); pressing Start/A enters the shift scene.
- **Notes:** Can be text-only initially.

### A-03 — Office background + camera

- **Done when:** Shift scene shows a placeholder office BG larger than 240×160; camera follows player within bounds.
- **Notes:** Match 3/4 depth feel; placeholder art fine if proportions allow up/down movement lanes.

### A-04 — Player movement + collision

- **Done when:** D-pad moves player in four directions; cannot walk through desk/wall collision boxes.
- **Notes:** Tunable speed constant.

### A-05 — Desk entities (×4)

- **Done when:** Four desks exist with positions, interact ranges, and idle vs broken visual states (placeholder frames OK).
- **Notes:** Data-driven desk table preferred over hardcoding in `main`.

### A-06 — Shift timer HUD

- **Done when:** Configurable shift length counts down on HUD; hits zero ends the shift.
- **Notes:** Length as a named constant for easy tweaking.

### A-07 — Ticket model + spawner

- **Done when:** Tickets spawn on a curve during the shift, bind to free desks, appear in game state (not necessarily UI yet).
- **Notes:** No tickets at t=0; spawn params tweakable.

### A-08 — Notepad UI

- **Done when:** Button toggles notepad overlay listing active tickets (desk/person, issue line, urgency).
- **Notes:** Visual nod to Sips notepad (`docs/references/sips-notepad-style.png`).

### A-09 — World cues

- **Done when:** Broken desks show a clear PC state; off-screen tickets get an edge indicator toward their desk.
- **Notes:** Icon vs face can be placeholder shapes.

### A-10 — Hold-to-reboot

- **Done when:** In range + hold button fills a progress bar and clears the ticket; release/leave cancels progress; PC returns to idle.
- **Notes:** Phase A only reboot type.

### A-11 — Soft urgency

- **Done when:** Open tickets increase urgency over time; notepad and/or cues reflect it; no game over from urgency.
- **Notes:** Prepares Phase B timeout/fail accounting.

### A-12 — Shift end + retry

- **Done when:** On timer end, show fixed count vs still-open count; input returns to title or restarts shift.
- **Notes:** Full ✓/✗ + 75% gate is Phase B — keep this summary minimal.

### A-13 — Phase A pass (integration)

- **Done when:** All acceptance criteria above verified in mGBA; this checklist fully `[x]`; short note in README how to build & run.
- **Notes:** Human verification OK; list any known jank for Phase B.
