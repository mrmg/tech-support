# Phase B — Shift results

**Unlock:** Phase A complete (A-01…A-13 all `- [x]`). **Unlocked.**

## Goal

End-of-**shift** evaluation: results notepad with ✓ fixed / ✗ failed, completion percentage, pass threshold (default 75%), retry on fail.

## In scope

- Classify tickets **at shift end only** (not mid-shift expiry)
- Persist enough ticket identity for a results list (desk, issue line, outcome)
- Results notepad UI (✓ / ✗)
- Pass/fail against completion %
- Retry shift / return to title
- Optional “don’t come back tomorrow” **flavour copy** on fail (still just retry — not Phase G sacking)

## Out of scope

- Multi-day campaign progression (Phase C)
- Mid-shift ticket removal / unfixable failed state
- Campaign-ending sacking (Phase G)
- Changing Phase A mid-shift urgency / hold-to-fix rules

## Failure timing (locked)

- During the shift: tickets stay fixable even at max urgency (urgency = warning).
- **When the shift timer hits zero:** every ticket that spawned is **✓** (fixed during the shift) or **✗** (still open / not fixed).
- No mid-shift transition to an unfixable “failed” state.

## Completion % (locked)

```
completion_percent = (fixed_count / (fixed_count + failed_count)) * 100
```

With end-of-shift classification, that equals `fixed_count / total_tickets_spawned * 100`.

If **zero** tickets spawned, treat as **pass** (100%) so empty shifts don’t soft-lock — document in code comments.

**Pass threshold:** default **75** (named configurable constant, e.g. `shift_results::pass_threshold_percent`).

## Baseline from Phase A

Current end screen (A-12): Fixed X / Still open Y; **A** retry; **B** title. Phase B replaces/enhances this with per-ticket ✓/✗, %, and pass/fail copy. Mid-shift behaviour stays as in Phase A.

## Acceptance criteria

- [x] At shift end, each spawned ticket is shown as ✓ or ✗
- [x] Completion % uses the formula above; threshold default 75
- [x] Below threshold → fail/retry messaging (flavour OK); at/above → pass messaging
- [x] Tickets are never removed as failed before the bell
- [x] **A** retries shift; **B** returns to title (same as A-12 inputs unless noted)

## Tickets

### - [x] B-01 — End-of-shift ticket classification

- **Done when:** On shift end, every ticket that spawned during the shift is classified **fixed** or **failed**; failed means “not fixed before the bell,” not “expired mid-shift.” Classification data is available to the results UI (not only aggregate counts).
- **Notes:** Soft urgency from A still only affects visuals during the shift. Need a spawn log / history (desk id, issue label, whether cleared before bell). Open tickets at the bell → failed; cleared via hold-to-reboot → fixed. Do not remove tickets mid-shift.
- **mGBA:** Temporarily lower `shift::duration_seconds` if needed → spawn ≥1 ticket → fix some, leave some open → at 0:00 verify classification data exists for each spawned ticket (debug text OK until B-02).
- **Implemented:** `ticket::history_entry` spawn log on `ticket::spawner` (desk id, issue type/label, outcome). `clear_desk` marks fixed; `classify_at_bell()` turns remaining pending → failed. Results UI is B-02.

### - [x] B-02 — Results notepad

- **Done when:** Shift-end screen lists each spawned ticket with ✓ or ✗ (notepad styling OK — can reuse/adapt A-08 notepad look).
- **Notes:** Show desk/person label + short issue line + outcome. Scroll or truncate if many tickets (GBA sprite limits — keep to a small max visible with “+N more” if needed). Replace bare Fixed/Open summary as the primary end UI (aggregates can remain as a header).
- **mGBA:** End shift with mixed fixed/open → see per-ticket ✓/✗ list.
- **Implemented:** `notepad::results_overlay` reuses `notepad_bg` + ink palette; lists desk + issue + OK/X (font has no ✓/✗ glyphs); `F:n O:n` header; max 4 rows + `+N more`; A retry / B title unchanged.

### - [x] B-03 — Completion % + pass threshold

- **Done when:** End screen shows completion % using the locked formula; configurable threshold default **75**; pass vs fail messaging differs (fail may use “don’t come back tomorrow” flavour — **not** a sacked game over).
- **Notes:** Named constant for threshold. Zero spawns → 100% / pass. Wire **A** = retry shift, **B** = title on both pass and fail (unless you add a one-line hint that fail still retries the same shift — no campaign yet).
- **mGBA:** End with under 75% → fail copy + A/B; end with 75% or more → pass copy + A/B. Threshold constant easy to find/edit.
- **Implemented:** `shift_results::pass_threshold_percent` (default 75) + `evaluate()`; results notepad shows `F:n O:n xx%` and pass (“PASS - good enough”) vs fail (“Don't come back tomorrow”); zero spawns → 100%/pass; A retry / B title unchanged on both.

### - [x] B-04 — Phase B pass (integration)

- **Done when:** All Phase B acceptance criteria verified; tickets B-01…B-03 are `- [x]`; README notes Phase B behaviour; known jank listed for Phase C.
- **Notes:**
  - **Verification:** B-01…B-03 already `- [x]`. Clean `make -j$(sysctl -n hw.ncpu)` → `tech-support.gba` (~153 KB, title `TECHSUPPORT` / `TS01`). Code review of `ticket::spawner` (history + `classify_at_bell` only at bell; no mid-shift fail/removal), `shift_results::evaluate` / `pass_threshold_percent` (75; zero spawns → 100%/pass), `notepad::results_overlay` (OK/X list, %, pass/fail copy), and `shift_scene` (Select pause + hold-A path unchanged; A retry / B title) confirms all acceptance paths. Homebrew mGBA launches the ROM (exit 0) but has **no headless/screenshot CLI**; full interactive click-through needs a human with a display.
  - **Human click-through still useful:** title→A→ tickets spawn → fix some / leave some → Select notepad still pauses → hold-A still clears → leave open for urgency → 0:00 → results notepad OK/X → `F:n O:n xx%` → pass or fail copy → A retry / B title. For a faster check, temporarily lower `shift::duration_seconds`.
  - **Known jank for Phase C:** results use **OK/X** (font has no ✓/✗ glyphs); list caps at **4 rows** + `+N more` (no scroll); 120s shift is slow to playtest under/over 75%; integer % truncates (e.g. 2/3 → 66%); fail copy (“Don't come back tomorrow”) still same-shift retry (no campaign day yet); spawn history soft-caps at 32; Phase A leftovers remain (placeholder art, no SFX/music, Desk N labels only, A overloaded, interact AABB vs art, progress bar / edge-icon flicker, notepad snapshot on open, soft urgency flash subtle, desk-corner slide snags).
- **mGBA checklist:** title→A→ tickets spawn → fix some / leave some → Select notepad still pauses → hold-A still clears → 0:00 → results notepad ✓/✗ → % → pass or fail copy → A retry / B title.
