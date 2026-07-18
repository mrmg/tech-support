# Phase H — Juice & ship

**Unlock:** Phase G complete (full fantasy through G).

## Goal

Polish: audio, light juice, credits, difficulty sanity, known-issues doc. Real-hardware pass is best-effort (document if no flashcart available).

## Acceptance criteria

- [x] Pleasant audio bed + key SFX (or documented stub if assets blocked)
- [x] Runs acceptably on emulator; real hardware noted if tested
- [x] Known issues documented

## Tickets

### - [x] H-01 — Key SFX

- **Done when:** SFX for at least: pickup, fix complete, ticket spawn or UI open, shift end / sacked (subset OK if channels limited).
- **Notes:** Short wav via Butano audio pipeline; silence-safe if file missing. Placeholder tones in `audio/sfx_*.wav` (8-bit mono 22050 Hz). `sfx::` wrappers use `bn::sound::play_optional` (channel-full = silence) and `__has_include("bn_sound_items.h")` no-ops if the audio folder has no WAVs.
- **mGBA:** Hear SFX during play.

### - [x] H-02 — Music bed

- **Done when:** Looping music or title sting plays during shift or title (Maxmod/Butano path).
- **Notes:** Keep ROM size reasonable; placeholder OK. Tiny looping `audio/music_shift.mod` (~2 KB) via Maxmod → `bn::music_items::music_shift`. `music::play_bed()` / `music::stop()` mirror `sfx::` (`__has_include("bn_music_items.h")` no-ops if missing). Plays on title + during shift; stops at shift-over.
- **mGBA:** Hear music.

### - [x] H-03 — Credits screen

- **Done when:** From title (or after campaign complete), credits list engine/assets/thanks; return with B/A.
- **Notes:** Text credits fine. Notepad-ish `run_credits_scene()` (engine / assets / thanks). Title **START** opens credits; after Day 5 pass, credits run before Day 1 reset. A/B/Start dismiss.
- **mGBA:** Open credits → back to title.

### - [x] H-04 — Difficulty / constants pass

- **Done when:** Review and tweak shift length, spawn curves, hold times, pass threshold, reputation rates for playability; document final defaults in README or `include/` comments.
- **Notes:** No new systems. Softened Day 1 spawn (`6/20/2/10`, ~9 tickets) and Day 5 min gap (5s); anger fail base **12** (was 15); starting toner **4**; kept 120s / 75% / 2s holds / shop prices. Table + knobs documented in README **Difficulty defaults (H-04)** and `campaign.cpp` / header comments.
- **mGBA:** A full Day 1 feels fair.

### - [x] H-05 — Phase H pass (ship checklist)

- **Done when:** Acceptance criteria met; H-01…H-04 `- [x]`; README “how to play” complete; `docs/known-issues.md` (or phase-H notes) lists remaining issues; phases README marks H complete.
- **Notes:**
  - **Verification:** H-01…H-04 already `- [x]`. Acceptance criteria: placeholder audio bed + key SFX in `audio/`; emulator build via `make` → `tech-support.gba`; **real GBA untested** (no flashcart); remaining jank in [`docs/known-issues.md`](../known-issues.md). README **How to play** covers title→day→shift→results→shop→credits. Phases README marks H **Complete**. Scene link path: `main` → `run_title_scene` → `run_shift_scene` → day intro → shift → results → optional `run_shop_scene` / `run_credits_scene` / `run_sacked_scene`.
  - **Build:** `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba`. Smoke: title (**START** credits) → **A** → Day intro → shift → results → mid-pass shop → (Day 5 pass) credits.
- **mGBA:** Smoke full loop title→day→shift→results→shop(if F)→credits.
