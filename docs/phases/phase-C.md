# Phase C — Campaign days

**Unlock:** Phase B complete. **Active.**  
**Note:** Required before Phase G.

## Goal

Linear **days** (Day 1, Day 2, …). Each day is one **shift** (plus retries). Rising spawn pressure. Fail a day → retry that day. Pass uses Phase B’s % gate; fail copy can keep “don’t come back tomorrow” flavour. This is **not** Phase G sacking.

## In scope

- Day index (start at Day 1)
- UI shows day number distinct from shift timer
- Per-day spawn/timer (or spawn-only) parameters with rising difficulty across early days
- Pass → advance to next day; fail → retry same day
- Simple day intro / between-day messaging
- Finite early campaign length (e.g. **5 days**) then a clear “campaign complete” (or loop day 5 params) — document constant

## Out of scope

- Between-day shop (Phase F)
- Campaign-ending reputation sacking (Phase G)
- New ticket types / rooms / carry (D/E)

## Terminology

| Term | Meaning |
|------|---------|
| **Day** | Campaign node (Day 1…N) |
| **Shift** | The timed play session for the current day |

## Acceptance criteria

- [ ] Completing a day (pass) unlocks the next
- [ ] Failing keeps you on the same day (retry shift)
- [ ] Difficulty trends upward across early days
- [ ] UI distinguishes day number from shift timer

## Tickets

### - [x] C-01 — Campaign day state

- **Done when:** A campaign module tracks `current_day` (starting at 1) and exposes helpers to read it / advance / reset for a new run. Title→shift path uses this state (even if UI still says little).
- **Notes:** Session memory is enough (SRAM optional, not required). Named constant for max days (suggest **5**).
- **mGBA:** Not much visible yet — prepare for C-02; debug text of day on shift OK.
- **Implemented:** `campaign` module (`include/campaign.h`, `src/campaign.cpp`) with `current_day` (boot/reset → 1), `max_days` (5), `reset()` / `advance()` / `current_day()`. `main` resets on boot; shift HUD debug-labels **Day N** from campaign state (title→A→shift).

### - [ ] C-02 — Day number HUD

- **Done when:** During a shift, HUD shows **Day N** clearly separate from the mm:ss shift timer.
- **Notes:** Keep sprite budget reasonable; don’t collide with timer layout.
- **mGBA:** title→A→ see Day 1 (or current) + timer both visible.

### - [ ] C-03 — Per-day difficulty parameters

- **Done when:** Day index selects spawn (and optionally shift length) parameters; early days get harder (faster/more spawns or shorter gaps). Constants are data-driven / easy to tweak.
- **Notes:** Do not break Phase A/B systems — feed params into existing `ticket::spawn::*` (or parallel per-day table). Day 1 can match current baseline; later days tighter.
- **mGBA:** Day 1 feels like today; jump current_day (debug) or advance and feel denser spawns on later days.

### - [ ] C-04 — Pass advances day / fail retries day

- **Done when:** Phase B **pass** advances `current_day` (or completes campaign on last day); **fail** keeps the same day and retries the shift. End/results copy can mention Day N.
- **Notes:** A = retry (same day on fail; or continue/next on pass as appropriate); B = title. On pass after final day → “campaign complete” then B/title or A to restart campaign from Day 1.
- **mGBA:** Fail → still Day N; Pass → Day N+1 on next shift; last day pass → complete message.

### - [ ] C-05 — Day intro / title campaign flow

- **Done when:** Starting a shift shows a brief **Day N** intro (or title shows day); new game resets to Day 1; returning from title continues current day if mid-campaign (session state).
- **Notes:** Keep intro short (Press A). No save file required.
- **mGBA:** Boot→A→ Day 1 intro→shift; after pass→ Day 2 intro; B to title→A resumes current day.

### - [ ] C-06 — Phase C pass (integration)

- **Done when:** All Phase C acceptance criteria verified; C-01…C-05 `- [x]`; README notes campaign days; known jank listed for Phase D.
- **Notes:** Clean make + code review; note human mGBA path. Confirm Phase B results still work.
- **mGBA checklist:** Day HUD → rising difficulty → fail stays on day → pass advances → complete after last day → title resume behaviour.
