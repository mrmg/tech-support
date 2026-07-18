# Known issues

Ship notes for **Tech Support** after Phase H. None of these block the A–H fantasy loop in mGBA.

## Hardware

- **Real GBA untested.** No flashcart pass in this project session. Emulator target: **mGBA**. If you run on hardware, note save type (none / no SRAM) and any audio/timing quirks here.

## Audio / juice

- SFX and music are **placeholder** tones / a tiny looping MOD (`audio/sfx_*.wav`, `audio/music_shift.mod`).
- Sound uses `play_optional` — when all Direct Sound channels are busy, SFX are silently dropped.
- No dedicated warn sting when anger hits 80+ (sack / shift-end / UI-open SFX cover those moments).

## UX / discoverability

- **A** is overloaded: closet pick/swap, desk hold, rack hold, shop buy, and confirm on title / day intro / results / sacked / credits.
- Title **SELECT** = new game and **START** = credits are easy to miss (prompts on title help).
- Day intro is bare text (“Day N” / Press A).
- Results use **OK/X** (font has no ✓/✗); list caps at **4 rows** + `+N more` (no scroll).
- Integer completion % truncates (e.g. 2/3 → 66%).
- No numeric anger on the shift HUD (only red **HR!** at 80+); full **Anger N/100** is on results. Title does not show anger. No anger bar.
- No in-shift budget / stock HUD (pending counts appear on shop rows only).
- “HR is watching” can sit near `+N more` when the ticket list overflows.
- Results → **SACKED** is two screens in a row.

## Systems / balance

- **No SRAM** — boot always Day 1; mid-campaign resume is session-only (B→title keeps the day until quit).
- All days are **120s**; sack / Day 5 playtests are slow without cheats.
- Strong-pass anger relief is only **−5** (slow recovery vs fail spikes).
- Fail / title exit / final-day pass **skip the shop**; pass→title still earns and can advance the day without a between-day spend.
- Zero-stock closet **A** and can't-afford shop **A** are silent (no deny flash).
- Carry pickup does not reserve stock until install (last unit still counts as on-hand while held).
- Final-day pass earn is wiped by campaign/`inventory` reset after credits.

## Multi-room / parts

- Rack hold-A works even with no open `needs_server_reset` (sets green until a later ticket re-arms).
- One rack clear fixes **all** open server-reset tickets at once.
- Cross-room edge icons can stack on the same door when several tickets point there.
- Instant room swap (no fade).
- Wrong / missing part at desk is silent (hold resets; no deny SFX).
- Desk with `reset server` still shows a broken PC — hold-A at the desk will not clear it (use the rack).

## Art / presentation

- Placeholder office / server / desk / player / part / credits art throughout.
- Desk interact AABB can feel larger than the desk art; progress bar / edge-icon flicker near view edges; desk-corner collision slide can snag.
- Soft urgency flash/scale is subtle.
- Notepad is a snapshot on open (OK while paused).
- Desk labels are “Desk N” only (no coworker faces/names).
- Spawn history soft-caps at 32 tickets per shift.

## Out of scope (by design)

- No save battery / SRAM campaign persistence.
- No final art pack or production audio.
- No online / multiplayer features.
