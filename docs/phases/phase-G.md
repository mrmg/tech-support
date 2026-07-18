# Phase G — Reputation & sacking

**Unlock:** Phase C complete. Prefer after F. **Requires campaign days.**  
**Complete.**

## Goal

Reputation/anger accumulates across days from failures. Cross threshold → **sacked** game over (run ends). Distinct from per-day fail/retry.

## Distinct from B/C

| System | Outcome |
|--------|---------|
| B/C fail | Retry same day |
| G sacked | Campaign over → title / new game |

## Acceptance criteria

- [x] Persistent reputation across days
- [x] Crossing threshold ends the run with a clear sacked game over
- [x] Player can see danger before it happens
- [x] Per-day fail/retry still works below the sack threshold

## Tickets

### - [x] G-01 — Reputation model

- **Done when:** Reputation (or anger) value persists across days in the session; worsens on day fail (and optionally on high failed-ticket counts); improves slightly on strong pass (optional). Named sack threshold.
- **Notes:** Show on results or HUD. Session module `reputation` (anger 0…`sack_threshold` 100). Fail: `+15 + failed*2`; strong pass (≥90%): `-5`. Reset with campaign/inventory (boot / title SELECT / campaign-complete / sacked). Results header shows `Anger N/100`. G-02/G-03 wired.
- **mGBA:** Fail a day → reputation worse (number/bar visible).

### - [x] G-02 — Danger warning UI

- **Done when:** When reputation is near threshold, clear warning on results and/or HUD (“HR is watching” / anger bar in red).
- **Notes:** Named `warning_threshold` = `sack_threshold - danger_margin` (80). `near_sack()` → results: red Anger + “HR is watching”; shift HUD: red “HR!”.
- **mGBA:** Near threshold → warning visible.

### - [x] G-03 — Sacked game over

- **Done when:** Crossing threshold after a day’s resolution shows a distinct **SACKED** game-over screen; A/B returns to title and resets campaign (+ reputation). Not the same as “don’t come back tomorrow” retry.
- **Notes:** Evaluate at end-of-day / results dismiss — not mid-shift. After `apply_shift_reputation`, results dismiss → if `at_sack_threshold()` → `run_sacked_scene` then `campaign`/`inventory`/`reputation` reset → title (skips retry/shop). Below threshold: fail still retries same day.
- **mGBA:** Push reputation over limit → sacked → title → new run Day 1.

### - [x] G-04 — Phase G pass (integration)

- **Done when:** Acceptance criteria met; G-01…G-03 `- [x]`; README updated; jank for H listed.
- **Notes:**
  - **Verification:** G-01…G-03 already `- [x]`. Acceptance criteria confirmed by code-path review: session `reputation` anger persists across days (no reset on advance / B→title); `apply_shift_reputation` after each shift (`+15 + failed*2` fail, `-5` on ≥90% pass); results show `Anger N/100`; `near_sack` (≥80) → red Anger + “HR is watching” on results and red `HR!` HUD; after results dismiss `at_sack_threshold()` (≥100) → `run_sacked_scene` then `campaign`/`inventory`/`reputation` reset → title (skips retry/shop); below threshold fail still retries same day; resets on boot / title SELECT / campaign-complete / sacked.
  - **Build:** `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba`. Fail days until Anger climbs; at 80+ see red / “HR is watching” / `HR!`; push to 100 → results → **SACKED** → title Day 1; below 100 fail still **A: Retry**.
  - **Known jank for Phase H:** no numeric anger on shift HUD (only `HR!` at 80+; full `Anger N/100` on results); no anger bar (text + red ink only); title does not show anger; sack needs several fails (~+15 each) and **120s** days make threshold playtests slow; results→SACKED is two screens; “HR is watching” can sit near `+N more` when the ticket list overflows; strong-pass relief only **-5** (slow recovery); no SFX/music on warn/sack (H-01/H-02); Phase F leftovers remain (silent zero-stock / can't-afford, pass→title skips shop, no budget/stock HUD, carry does not reserve stock); Phase E leftovers remain (rack hold with no server ticket, one rack clears all server-resets, door cue stacking, instant room swap, silent wrong/missing part, placeholder art); Phase C leftovers remain (no SRAM, bare day intro, OK/X results, 4-row cap, A/SELECT discoverability).
- **mGBA:** Fail/retry still works when below threshold; sack ends run.
