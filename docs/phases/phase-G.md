# Phase G — Reputation & sacking

**Unlock:** Phase C complete. Prefer after F. **Requires campaign days.**

## Goal

Reputation/anger accumulates across days from failures. Cross threshold → **sacked** game over (run ends). Distinct from per-day fail/retry.

## Distinct from B/C

| System | Outcome |
|--------|---------|
| B/C fail | Retry same day |
| G sacked | Campaign over → title / new game |

## Acceptance criteria

- [ ] Persistent reputation across days
- [ ] Crossing threshold ends the run with a clear sacked game over
- [ ] Player can see danger before it happens
- [ ] Per-day fail/retry still works below the sack threshold

## Tickets

### - [ ] G-01 — Reputation model

- **Done when:** Reputation (or anger) value persists across days in the session; worsens on day fail (and optionally on high failed-ticket counts); improves slightly on strong pass (optional). Named sack threshold.
- **Notes:** Show on results or HUD.
- **mGBA:** Fail a day → reputation worse (number/bar visible).

### - [ ] G-02 — Danger warning UI

- **Done when:** When reputation is near threshold, clear warning on results and/or HUD (“HR is watching” / anger bar in red).
- **Notes:** Must be readable before sack.
- **mGBA:** Near threshold → warning visible.

### - [ ] G-03 — Sacked game over

- **Done when:** Crossing threshold after a day’s resolution shows a distinct **SACKED** game-over screen; A/B returns to title and resets campaign (+ reputation). Not the same as “don’t come back tomorrow” retry.
- **Notes:** Evaluate at end-of-day / results dismiss — not mid-shift.
- **mGBA:** Push reputation over limit → sacked → title → new run Day 1.

### - [ ] G-04 — Phase G pass (integration)

- **Done when:** Acceptance criteria met; G-01…G-03 `- [x]`; README updated; jank for H listed.
- **mGBA:** Fail/retry still works when below threshold; sack ends run.
