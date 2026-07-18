# Known issues

Ship notes for **Tech Support** after Phase J. None of these block the A–J fantasy loop in mGBA.

## Hardware

- **Real GBA untested.** No flashcart pass in this project session. Emulator target: **mGBA**. If you run on hardware, note save type (none / no SRAM) and any audio/timing quirks here.

## Audio / juice

- SFX and music are **placeholder** tones / a tiny looping MOD (`audio/sfx_*.wav`, `audio/music_shift.mod`).
- Sound uses `play_optional` — when all Direct Sound channels are busy, SFX are silently dropped.
- No dedicated warn sting when anger hits 80+ (sack / shift-end / UI-open SFX cover those moments).
- Wrong / missing part and empty-bin deny flash have **no deny SFX** (visual only).

## UX / discoverability

- **A** is overloaded: supply-bin pick/swap, desk/printer hold, coffee brew, rack hold, shop buy, and confirm on title / day intro / results / sacked / credits.
- Title **SELECT** = new game and **START** = credits are easy to miss (prompts on title help).
- Day intro is bare text (“Day N” + one-line lesson / Press A).
- Results use **OK/X** (font has no ✓/✗); list caps at **4 rows** + `+N more` (no scroll).
- Integer completion % truncates (e.g. 2/3 → 66%).
- No numeric anger on the shift HUD (only red **HR!** at 80+); full **Anger N/100** is on results. Title does not show anger. No anger bar.
- No in-shift budget / stock HUD (bin count labels are world-space only; pending counts appear on shop rows).
- Energy bar is small (top-left under Day); easy to miss until you sprint dry.
- “HR is watching” can sit near `+N more` when the ticket list overflows.
- Results → **SACKED** is two screens in a row.

## Teaching days / equipment (Phase J)

- Teaching shifts are short (**45 / 55 / 60 / 60s**); Day 1 can feel sparse if you clear the first reboot slowly.
- Bins start **empty**; Day 2 toner / Day 3 PSU tutorial grants are free and silent (no delivery sting) — look at the bin count.
- Tutorial retry re-grant only fires while the teaching install is incomplete and the matching bin is empty (carry still holding the part blocks a second grant until you return/install/lose it).
- Printers and coffee use **placeholder** sprites (`printer.bmp`, `coffee.bmp`); collide/interact AABBs can feel larger than the art.
- Two printers at opposite ends — camera may not show both; rely on edge cues for off-screen toner.
- During a Day 4 **server outage**, every desk shows broken + server badge even if that desk has no personal ticket; toner printers are unaffected and may still spawn.
- Outage is one history/results line (`Server outage`); rack clear records one Fixed even though every PC looked broken.
- Coffee station sits in the lower aisle — can compete with desk paths; hold-A brew wins over bin/desk A when in coffee range.
- Sprint (**R**) drains in ~3s of held sprint; walk never drains. Exhausted half-speed has no separate SFX/pose beyond the energy bar.
- Notepad open pauses sprint drain and coffee brew (by design); easy to forget energy is paused while reading.
- `campaign::set_day` exists for playtest builds but is **not** bound to any keypad shortcut in ship builds.

## Systems / balance

- **No SRAM** — boot always Day 1; mid-campaign resume is session-only (B→title keeps the day until quit).
- Strong-pass anger relief is only **−5** (slow recovery vs fail spikes).
- Fail / title exit / final-day pass **skip the shop**; pass→title still earns and can advance the day without a between-day spend.
- Can't-afford shop **A** is silent (no deny flash). Empty supply-bin **A** flashes deny (J-04).
- Carry pickup does not reserve stock until install (last unit still counts as on-hand while held).
- Final-day pass earn is wiped by campaign/`inventory` reset after credits.

## Multi-room / parts

- Rack hold-A works even with no open `needs_server_reset` (sets green until a later ticket re-arms).
- One rack clear fixes the single global outage ticket (and any legacy open server-reset rows) at once.
- Cross-room edge icons can stack on the same door when several tickets point there.
- Instant room swap (no fade).
- Desk with a personal ticket still shows broken during an outage; after the outage clears, personal tickets remain until fixed normally.
- Desk hold-A never clears a server outage (use the rack).

## Art / presentation

- Placeholder office / server / desk / printer / coffee / player / part / credits art throughout.
- Desk/printer interact AABB can feel larger than the art; progress bar / edge-icon flicker near view edges; desk-corner collision slide can snag.
- Soft urgency flash/scale is subtle.
- Notepad is a snapshot on open (OK while paused).
- Desk labels are “Desk N” only (no coworker faces/names); printers are “Printer 1/2”.
- Spawn history soft-caps at 32 tickets per shift.

## Out of scope (by design)

- No save battery / SRAM campaign persistence.
- No final art pack or production audio.
- No online / multiplayer features.
- Future incident kinds left for later phases: **paper jam**, **software install**, **password reset**, **network cable** (not in Phase J).
