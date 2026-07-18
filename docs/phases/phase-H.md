# Phase H — Juice & ship

**Unlock:** Phase G complete (full fantasy through G).

## Goal

Polish: audio, light juice, credits, difficulty sanity, known-issues doc. Real-hardware pass is best-effort (document if no flashcart available).

## Acceptance criteria

- [ ] Pleasant audio bed + key SFX (or documented stub if assets blocked)
- [ ] Runs acceptably on emulator; real hardware noted if tested
- [ ] Known issues documented

## Tickets

### - [ ] H-01 — Key SFX

- **Done when:** SFX for at least: pickup, fix complete, ticket spawn or UI open, shift end / sacked (subset OK if channels limited).
- **Notes:** Short wav via Butano audio pipeline; silence-safe if file missing.
- **mGBA:** Hear SFX during play.

### - [ ] H-02 — Music bed

- **Done when:** Looping music or title sting plays during shift or title (Maxmod/Butano path).
- **Notes:** Keep ROM size reasonable; placeholder OK.
- **mGBA:** Hear music.

### - [ ] H-03 — Credits screen

- **Done when:** From title (or after campaign complete), credits list engine/assets/thanks; return with B/A.
- **Notes:** Text credits fine.
- **mGBA:** Open credits → back to title.

### - [ ] H-04 — Difficulty / constants pass

- **Done when:** Review and tweak shift length, spawn curves, hold times, pass threshold, reputation rates for playability; document final defaults in README or `include/` comments.
- **Notes:** No new systems.
- **mGBA:** A full Day 1 feels fair.

### - [ ] H-05 — Phase H pass (ship checklist)

- **Done when:** Acceptance criteria met; H-01…H-04 `- [x]`; README “how to play” complete; `docs/known-issues.md` (or phase-H notes) lists remaining issues; phases README marks H complete.
- **Notes:** Note if real GBA untested.
- **mGBA:** Smoke full loop title→day→shift→results→shop(if F)→credits.
