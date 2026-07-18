# Phase C — Campaign days

**Unlock:** Phase B complete. **Complete.**  
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

- [x] Completing a day (pass) unlocks the next
- [x] Failing keeps you on the same day (retry shift)
- [x] Difficulty trends upward across early days
- [x] UI distinguishes day number from shift timer

## Tickets

### - [x] C-01 — Campaign day state

- **Done when:** A campaign module tracks `current_day` (starting at 1) and exposes helpers to read it / advance / reset for a new run. Title→shift path uses this state (even if UI still says little).
- **Notes:** Session memory is enough (SRAM optional, not required). Named constant for max days (suggest **5**).
- **mGBA:** Not much visible yet — prepare for C-02; debug text of day on shift OK.
- **Implemented:** `campaign` module (`include/campaign.h`, `src/campaign.cpp`) with `current_day` (boot/reset → 1), `max_days` (5), `reset()` / `advance()` / `current_day()`. `main` resets on boot; shift HUD debug-labels **Day N** from campaign state (title→A→shift).

### - [x] C-02 — Day number HUD

- **Done when:** During a shift, HUD shows **Day N** clearly separate from the mm:ss shift timer.
- **Notes:** Keep sprite budget reasonable; don’t collide with timer layout.
- **mGBA:** title→A→ see Day 1 (or current) + timer both visible.
- **Implemented:** Shift HUD draws **Day N** top-left from `campaign::current_day()` in its own sprite vector; mm:ss stays top-center and redraws independently (no shared clear / layout collision).

### - [x] C-03 — Per-day difficulty parameters

- **Done when:** Day index selects spawn (and optionally shift length) parameters; early days get harder (faster/more spawns or shorter gaps). Constants are data-driven / easy to tweak.
- **Notes:** Do not break Phase A/B systems — feed params into existing `ticket::spawn::*` (or parallel per-day table). Day 1 can match current baseline; later days tighter.
- **mGBA:** Day 1 feels like today; jump current_day (debug) or advance and feel denser spawns on later days.
- **Implemented:** `campaign::day_difficulty` table in `campaign.cpp` (Day 1 = Phase A spawn baseline; days 2–5 tighter gaps). `ticket::spawn::params` + `spawner(params)` feed the curve; `shift_scene` selects via `difficulty_for_day(current_day())` (shift length included, currently 120s all days). `set_day` helper exists for playtest; title L/R jump not wired (see C-06 jank).

### - [x] C-04 — Pass advances day / fail retries day

- **Done when:** Phase B **pass** advances `current_day` (or completes campaign on last day); **fail** keeps the same day and retries the shift. End/results copy can mention Day N.
- **Notes:** A = retry (same day on fail; or continue/next on pass as appropriate); B = title. On pass after final day → “campaign complete” then B/title or A to restart campaign from Day 1.
- **mGBA:** Fail → still Day N; Pass → Day N+1 on next shift; last day pass → complete message.
- **Implemented:** `shift_scene::apply_shift_outcome` — fail leaves day; pass calls `campaign::advance()`; final-day pass shows “Campaign complete” then `campaign::reset()` (A → Day 1 shift, B → title Day 1). Results notepad header **Day N**; A prompt Retry / Next day / Restart.

### - [x] C-05 — Day intro / title campaign flow

- **Done when:** Starting a shift shows a brief **Day N** intro (or title shows day); new game resets to Day 1; returning from title continues current day if mid-campaign (session state).
- **Notes:** Keep intro short (Press A). No save file required.
- **mGBA:** Boot→A→ Day 1 intro→shift; after pass→ Day 2 intro; B to title→A resumes current day.
- **Implemented:** `day_intro_scene` (“Day N” + Press A) before every shift. Title shows session day; A continues without reset; SELECT = new game (`campaign::reset()`). Boot resets once; B→title keeps day.

### - [x] C-06 — Phase C pass (integration)

- **Done when:** All Phase C acceptance criteria verified; C-01…C-05 `- [x]`; README notes campaign days; known jank listed for Phase D.
- **Notes:**
  - **Verification:** C-01…C-05 already `- [x]`. Acceptance criteria (pass unlocks next; fail retries same day; difficulty trends up; Day N HUD ≠ timer) confirmed by code review of `campaign` (`current_day` / `advance` / `reset` / `max_days=5` / `day_difficulty` table), `shift_scene` (Day HUD + timer; `difficulty_for_day` → spawn params; `apply_shift_outcome`; day intro before each shift), `day_intro_scene`, `title_scene` (A continues session day; SELECT = new game), and `notepad::results_overlay` (Day N, OK/X, %, pass/fail/“Campaign complete”, A Retry/Next day/Restart). Phase B path intact: `classify_at_bell` only at 0:00; `shift_results::evaluate` / 75% threshold; zero spawns → 100%/pass; Select pause + hold-A clear unchanged.
  - **Build:** Clean `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make clean && make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba` (Homebrew/Nix `mgba` on PATH). No headless/screenshot CLI — interactive click-through needs a human with a display. Optional: temporarily shorten `campaign::day_difficulty::shift_duration_seconds` (or Day-1 row) for faster pass/fail checks, then restore **120**.
  - **Known jank for Phase D:** title L/R day jump claimed in C-03 notes was never wired (`campaign::set_day` unused — day jumps need a pass or code edit); all days still **120s** (spawn-only difficulty; slow to verify Day 5 / campaign complete); day intro is bare text (“Day N” / Press A); no SRAM (boot always Day 1; mid-campaign resume is session-only); results still **OK/X**, **4-row** cap + `+N more`, integer % truncation; history soft-cap **32**; final-day pass resets to Day 1 as soon as results dismiss (title after complete shows Day 1); SELECT new-game is easy to miss; Phase A/B leftovers remain (placeholder art, no SFX/music, Desk N labels, A overloaded across confirm/hold-reboot/retry/intro, interact AABB vs art, progress bar / edge-icon flicker, notepad snapshot on open, soft urgency, desk-corner slide snags).
- **mGBA checklist:** Day HUD → rising difficulty → fail stays on day → pass advances → complete after last day → title resume behaviour.
