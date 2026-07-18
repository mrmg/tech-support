# Phase G — Reputation & sacking

**Unlock:** Phase C complete (multi-day campaign exists). Phase F helpful but not strictly required.

## Goal

Anger/reputation accumulates **across days**. Too many failures → **sacked**: campaign-ending game over at the between-day / end-of-day moment — not a random mid-shift kill.

## Distinct from Phase B/C

| System | Phase | Outcome |
|--------|-------|---------|
| Pass / retry | B–C | Fail one shift/day → retry that day |
| Sacked | G | Reputation threshold → **run over**; back to title/new campaign |

Do not reuse “don’t come back tomorrow” as the only sacked screen without a clear game-over state.

## In scope

- Reputation meter
- Cross-day accumulation rules
- Game over / sacked screen
- Warnings before the final sack

## Out of scope

- Mid-shift instant sack with no warning (unless a later design explicitly adds a final warning spike)
- Permadeath meta beyond restart campaign

## Acceptance criteria

- [ ] Persistent reputation across days
- [ ] Crossing threshold ends the run with a clear sacked game over
- [ ] Player can see danger before it happens
- [ ] Per-day fail/retry still works independently below the sack threshold

## Tickets

_Expand when unlocked. Keep ticket headings as `### - [ ] G-xx — …`._  
_Requires C; do not start before campaign days exist._
