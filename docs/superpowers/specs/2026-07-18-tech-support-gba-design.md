# Tech Support (GBA) — Design Spec

**Date:** 2026-07-18  
**Status:** Draft for review  
**Platform:** Game Boy Advance (emulator first, real hardware later)  
**Engine:** Butano (modern C++)

## 1. Vision

You work in an office as tech support. During the workday, people’s computers break. You walk the floor, check a pull-up notepad of tickets, and fix issues. Early on that means standing at a desk and holding a button to “turn it off and on again.” Later, issues get more complex: fetch parts from storage, visit the server room, replace toner, manage stock and an IT budget between days, and risk getting sacked if too many coworkers stay angry.

Tone: light workplace comedy / chaos management (Overcooked / Moving Out energy), not a hard sim.

### Visual / UX reference (not a code fork)

Style inspiration is [foopod/sips](https://github.com/foopod/sips) — a Butano GBA game — **as art and UX reference only**. This project is a clean Butano template, not a fork of Sips.

- **Camera:** 3/4 side view with upward tilt (depth: move left/right and up/down on the floor plane).
- **Characters:** Slim, minimal GBA-scale sprites.
- **Ticket UI:** Pull-up **notepad** overlay (Sips “To Buy” pattern), not a CRT screen away from your desk.
- Reference screenshots in-repo: `docs/references/sips-office-style.png`, `docs/references/sips-notepad-style.png`.

## 2. Technical foundation

| Choice | Decision |
|--------|----------|
| Approach | Vision + phased roadmap docs; implement one phase at a time |
| Engine | **Butano** (high-level C++) |
| Rejected for V1 | Forking Sips; Rust/`agb` (unnecessary shoehorn); raw libtonc-first (good learning, slower ship); BPCore Lua (too small for this scope) |
| Tooling | mGBA for day-to-day + GDB; NanoBoyAdvance for accuracy checks; flashcart later |
| Art pipeline | Aseprite (or equivalent) → Butano graphics import |

Why Butano (July 2026): gbadev’s recommended high-level path; actively maintained; sprites/BGs/text/audio/asset pipeline match this game’s needs; closest to what a small team would pick to ship a GBA game without living in hardware registers.

## 3. Phased roadmap

Each phase is a shippable slice with goals that expand into implementation tickets under `docs/phases/`.

| Phase | Name | Goal |
|-------|------|------|
| **A** | Office & core loop (V1) | Walk a small office; tickets spawn over time; notepad + world cues; hold-to-reboot; soft urgency; timed shift |
| **B** | Shift results | End-of-shift notepad (✓/✗); completion % (e.g. ~75% pass); retry |
| **C** | Campaign days | Linear Day 1, Day 2…; rising pressure; fail = retry that day |
| **D** | Carry & parts | One carried item; storage; ticket types needing a part then install |
| **E** | Multi-room office | Server room / printer / larger floor; office grows or new sites |
| **F** | Stock & budget | Between-day ordering; shortages block fixes |
| **G** | Reputation & sacking | Cross-day anger; true game-over when sacked |
| **H** | Juice & ship | Audio, polish, difficulty, title/credits, hardware pass |

**V1 = Phase A only.** No campaign day chain until Phase C. Leftover “ordered parts from yesterday” is Phase F territory.

## 4. Phase A systems (V1)

### Office

- One scrollable floor, Sips-style 3/4 view.
- ~4 coworker desks with PCs; collision around furniture.
- Player is tech support; coworkers stay at desks for V1.

### Shift

- Configurable day length (tweakable constant).
- HUD shows time remaining.
- Day starts with **no** tickets (unless a later phase adds leftovers).

### Tickets

- Spawn on a schedule/curve during the shift (not all at t=0).
- Fields: desk/person, type (V1: reboot only), patience/urgency.
- Soft patience: urgency rises (notepad + world cue); **no mid-shift sacking**.

### Notepad

- Pull up anywhere (button TBD: Start/Select).
- Ruled notepad list: person/desk, short issue line, urgency.
- Pause vs soft-pause: prefer fair timing; decide in implementation if open notepad freezes the shift clock.

### World cues

- On-screen: broken PC state (smoke / error flash).
- Off-screen: edge indicator toward open tickets (icon vs face TBD).

### Fixing

- In range at desk, **hold** face button: progress bar → ticket clears → PC normal.
- Must keep holding in range (release/leave cancels or resets progress — still “hold to fix,” not a separate interrupt subsystem).

### End of Phase A

- Timer hits zero → simple “Shift over — fixed X / Y open” → title/retry.
- Full ✓/✗ notepad + ~75% pass gate = **Phase B**.

## 5. Documentation layout

```
docs/
  game-vision.md                 # Full dream / tone / references
  dev-workflow.md                # How to develop with Cursor (/loop, tickets)
  phases/
    README.md                    # Phase index
    phase-A.md … phase-H.md      # Scope + acceptance + ticket checklists
  references/                    # Style screenshots
  superpowers/specs/             # This design spec
```

### Ticket format

Each checklist item:

- **ID** — e.g. `A-03`
- **Title** — one line
- **Done when** — emulator-checkable acceptance
- **Notes** — constraints, art deps, tweakables

### Code modules (Phase A target shape)

Keep logic out of a single giant `main.cpp`:

- `player`, `office`, `tickets`, `notepad`, `fix_interaction`, `hud`, scenes (`title`, `shift`)

## 6. Cursor `/loop` development workflow

See **`docs/dev-workflow.md`** for the full playbook. Summary:

- Tickets live as checkboxes in `docs/phases/phase-*.md`.
- Use `/loop` so the agent repeatedly takes the **next open ticket**, implements it, builds, marks it done, then waits for the next tick.
- Optimal cadence for GBA/Butano work is roughly **10–15 minutes** per tick (or dynamic mode), **one ticket per tick**.

## 7. Out of scope for this spec

- Exact art production pipeline beyond “Sips-like + Butano import”
- Final button mapping, day length, and spawn curve numbers (tunable during Phase A)
- Music/SFX design (Phase H, light stubs OK earlier)
- Online features, multiplayer, save format details beyond “needed when campaign exists”

## 8. Success criteria for leaving Phase A

- Builds a `.gba` ROM with Butano and runs in mGBA.
- Player can walk a ~4-desk office with collision.
- Tickets spawn over a timed shift; notepad lists them; world cues point to issues.
- Hold-to-reboot clears a ticket.
- Soft urgency visible; shift end shows a simple summary.
- Phase A ticket checklist in `docs/phases/phase-A.md` is complete.
