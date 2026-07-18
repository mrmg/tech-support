# Phase B — Shift results

**Unlock:** Phase A acceptance criteria met (all Phase A tickets `- [x]`).

## Goal

End-of-**shift** evaluation: results notepad with ✓ fixed / ✗ failed, completion percentage, pass threshold (default 75%), retry on fail.

## In scope

- Classify tickets **at shift end only** (not mid-shift expiry)
- Results notepad UI (✓ / ✗)
- Pass/fail against completion %
- Retry shift / return to title
- Optional “don’t come back tomorrow” **flavour copy** on fail (still just retry — not Phase G sacking)

## Out of scope

- Multi-day campaign progression (Phase C)
- Mid-shift ticket removal / unfixable failed state
- Campaign-ending sacking (Phase G)

## Failure timing (locked)

- During the shift: tickets stay fixable even at max urgency (urgency = warning).
- **When the shift timer hits zero:** every ticket that spawned is **✓** (fixed during the shift) or **✗** (still open / not fixed).
- No mid-shift transition to an unfixable “failed” state.

## Completion % (locked)

```
completion_percent = (fixed_count / (fixed_count + failed_count)) * 100
```

With end-of-shift classification, that equals `fixed_count / total_tickets_spawned * 100`.

**Pass threshold:** default **75** (named configurable constant).

## Acceptance criteria

- [ ] At shift end, each spawned ticket is shown as ✓ or ✗
- [ ] Completion % uses the formula above; threshold default 75
- [ ] Below threshold → fail/retry messaging (no campaign advance; none exists yet)
- [ ] Tickets are never removed as failed before the bell

## Tickets

### - [ ] B-01 — End-of-shift ticket classification

- **Done when:** On shift end, every spawned ticket is marked fixed or failed; failed means “not fixed before the bell,” not “expired mid-shift.”
- **Notes:** Soft urgency from A still only affects visuals during the shift.

### - [ ] B-02 — Results notepad

- **Done when:** Shift-end screen lists each ticket with ✓ or ✗ (notepad styling OK).
- **Notes:**

### - [ ] B-03 — Pass threshold + retry

- **Done when:** Configurable % (default 75) gates pass vs fail copy; fail offers retry / title; pass can return to title until Phase C exists.
- **Notes:** Fail copy may say “don’t come back tomorrow” as flavour only — not a sacked game over.
