# Phase B — Shift results

## Goal

End-of-shift evaluation: notepad summary with ✓ fixed / ✗ failed (timed out), completion percentage, pass threshold (e.g. ~75%), retry on fail.

## In scope

- Ticket timeout → counts as failed at shift end (building on Phase A soft urgency)
- Results notepad UI
- Pass/fail against configurable completion %
- Retry shift / return to title

## Out of scope

- Multi-day campaign progression (Phase C)
- Mid-shift sacking (Phase G)

## Acceptance criteria

- [ ] Failed/expired tickets shown with ✗; fixed with ✓
- [ ] Completion % displayed; configurable pass threshold
- [ ] Fail path does not advance a campaign (none yet); clear retry

## Tickets

_To be expanded when Phase A is complete._

### B-01 — Ticket expiry → failed state

- **Done when:** Tickets that hit max urgency/timeout become failed and stop needing a fix.
- **Notes:**

### B-02 — Results notepad

- **Done when:** Shift end lists each ticket with ✓/✗.
- **Notes:**

### B-03 — Pass threshold

- **Done when:** Configurable % (default 75%) gates pass vs retry messaging.
- **Notes:**
