# Tech Support (GBA) — Design Spec

**Date:** 2026-07-18  
**Status:** Approved (docs consistency pass)  
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

## 3. Terminology

| Term | Meaning |
|------|---------|
| **Shift** | One timed play session on the office floor (Phase A+). Prefer this word over “day” until the campaign exists. |
| **Day** | A campaign node (Day 1, Day 2, …) introduced in **Phase C**. Each day is played as one shift (or a retry of that day’s shift). |
| **Pass / retry (B–C)** | End-of-shift (later: end-of-day) scoregate. Fail → retry that shift/day. Not a run-ending sack. |
| **Sacked (G)** | Campaign-ending game over from accumulated reputation. Separate system from per-shift pass/retry. |

## 4. Phased roadmap

Each phase is a shippable slice with goals that expand into implementation tickets under `docs/phases/`.

| Phase | Name | Goal |
|-------|------|------|
| **A** | Office & core loop (V1) | Walk a small office; tickets spawn over time; notepad + world cues; hold-to-reboot; soft urgency; timed shift |
| **B** | Shift results | End-of-shift notepad (✓/✗); completion % pass gate; retry |
| **C** | Campaign days | Linear Day 1, Day 2…; rising pressure; fail = retry that day |
| **D** | Carry & parts | One carried item; same-map storage closet; ticket types needing a part then install |
| **E** | Multi-room office | Separate rooms (server, printer, larger floor); office grows or new sites |
| **F** | Stock & budget | Between-day ordering; shortages block fixes |
| **G** | Reputation & sacking | Cross-day anger; true game-over when sacked |
| **H** | Juice & ship | Audio, polish, difficulty, title/credits, hardware pass |

**V1 = Phase A only.** No campaign until Phase C. Leftover “ordered parts from yesterday” is Phase F territory.

### Phase sequencing rules

**Recommended order:** A → B → C → D → E → F → G → H.

Allowed flexibility (explicit exceptions):

- **D before C:** If campaign is deferred, Carry & parts may follow B.
- **F after D without E:** Economy may land before multi-room if stock only needs the Phase D closet.
- **G requires C:** Reputation/sacking needs a multi-day campaign to matter.
- **E assumes D’s carry model** (or a minimal carry stub) so room-to-room fetch jobs work.

`docs/phases/README.md` is the index; each `phase-*.md` states its unlock rule.

## 5. Phase A systems (V1)

### Office

- One scrollable floor, Sips-style 3/4 view.
- ~4 coworker desks with PCs; collision around furniture.
- Player is tech support; coworkers stay at desks for V1.

### Shift

- Configurable **shift** length (named constant; suggested starting value: **120 seconds** real-time).
- HUD shows time remaining.
- Shift starts with **no** tickets (leftovers are a later phase).

### Tickets

- Spawn on a schedule/curve during the shift (not all at t=0). Suggested: first spawn after ~5s; then staggered.
- Fields: desk/person, type (V1: reboot only), patience/urgency.
- Soft patience: urgency rises (notepad + world cue); **no mid-shift fail, expiry, or sacking**. Tickets remain fixable until the shift timer ends.

### Notepad

- Toggle with **Select** (always available during a shift).
- Ruled notepad list: person/desk, short issue line, urgency.
- **While the notepad is open, the shift timer and urgency progression pause** (fair reading on GBA). Movement/fix are blocked until closed.

### World cues

- On-screen: broken PC state (smoke / error flash).
- Off-screen: edge indicator with a **simple issue icon** (not coworker faces) in Phase A.

### Fixing (hold-to-reboot)

- Stand in the desk’s interact range and **hold A**: progress bar fills → ticket clears → PC returns to idle.
- Progress advances only while **A is held and the player stays in range**.
- **Releasing A or leaving range resets progress to zero** (not a separate “interruptible channelled cast” system — you are simply no longer performing the hold).

### Controls (Phase A)

| Input | Action |
|-------|--------|
| D-pad | Move |
| **A** | Hold to reboot (in range); confirm on title / shift-end |
| **Select** | Toggle notepad |
| **Start** | Reserved (e.g. future pause menu); not notepad |

### End of Phase A

- Timer hits zero → simple “Shift over — fixed X / still open Y” → title or retry.
- No ✓/✗ per ticket, no % pass gate yet — that is **Phase B**.
- Tickets still open at the bell are counted as “still open,” not failed (failure accounting starts in B).

## 6. Phase B rules (locked early to avoid drift)

- During the shift, tickets stay fixable even at max urgency (urgency is a warning).
- **At shift end only:** every ticket that appeared is either **✓ fixed** or **✗ failed** (still open / not fixed in time).
- **Completion %** = `(fixed / (fixed + failed)) × 100`.  
  Equivalent: `fixed / total_tickets_that_spawned` when every ticket is classified ✓ or ✗ at the bell.
- **Pass threshold** default: **75%**. Configurable named constant.
- Fail → retry messaging (“don’t come back tomorrow” *flavour* for that shift). This is **not** Phase G sacking.

## 7. Storage vs rooms (D vs E)

- **Phase D:** Storage is a **closet / cupboard zone on the same office map** (walk across the floor, pick up a part).
- **Phase E:** **Separate rooms** (server room, printer area, larger floor plans, site upgrades). Off-screen cues must work across room transitions.

## 8. Documentation layout

```
docs/
  game-vision.md                 # Full dream / tone / references
  dev-workflow.md                # How to develop with Cursor (/loop, tickets)
  phases/
    README.md                    # Phase index + sequencing rules
    phase-A.md … phase-H.md      # Scope + acceptance + ticket checklists
  references/                    # Style screenshots
  superpowers/specs/             # This design spec
```

### Ticket format

Each implementation ticket in `phase-*.md` is a markdown checkbox:

```markdown
### - [ ] A-03 — Title here

- **Done when:** …
- **Notes:** …
```

Mark `- [x]` only when **Done when** is satisfied.

### Code modules (Phase A target shape)

Keep logic out of a single giant `main.cpp`:

- `player`, `office`, `tickets`, `notepad`, `fix_interaction`, `hud`, scenes (`title`, `shift`)

## 9. Cursor `/loop` development workflow

See **`docs/dev-workflow.md`** for the full playbook. Summary:

- Tickets are `- [ ]` checkboxes under each phase file.
- Use `/loop` so the agent takes the **next open ticket**, implements it, builds, marks it done, then waits.
- Cadence: **10–15 minutes** per tick (or dynamic), **one ticket per tick**.
- Run **A-01** (scaffold) in a normal chat first; start `/loop` from **A-02** (or after A-01 is checked).

## 10. Out of scope for this spec

- Full final art pack (placeholders OK in Phase A)
- Exact spawn-curve tuning beyond suggested starting constants
- Music/SFX design (Phase H; light stubs OK earlier)
- Online features, multiplayer; save format details until campaign (Phase C) needs them

## 11. Success criteria for leaving Phase A

- Builds a `.gba` ROM with Butano and runs in mGBA.
- Player can walk a ~4-desk office with collision.
- Tickets spawn over a timed shift; notepad lists them; world cues point to issues.
- Hold-to-reboot clears a ticket (hold-in-range; release/leave resets progress).
- Soft urgency visible; no mid-shift ticket failure.
- Shift end shows fixed vs still-open counts; can restart.
- All Phase A tickets in `docs/phases/phase-A.md` are `- [x]`.
