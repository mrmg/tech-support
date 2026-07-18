# Fun polish — readability, motion, juice

**Date:** 2026-07-18  
**Status:** Approved for autonomous implementation (player asked for no further design input; dinner-time execution).

## Problem

The campaign systems work, but the loop does not feel fun yet:

- First ticket always hits the top-left desk (deterministic free-desk scan).
- Issue types follow a fixed 5-cycle (learnable, not surprising).
- In-world desks all look like the same broken PC; type truth lives only in the notepad.
- Player is a static 1 px/frame sprite — movement feels like dragging a cursor.
- Carry is a corner HUD, not a physical prop.
- Wrong-part holds fail silently; success has almost no juice.
- Timer pressure is text-only.

## Goals

1. **Readable at a glance** — standing at a desk (or seeing it on-screen) shows *what it needs*.
2. **Surprising but fair** — desks and issue mix feel random within free slots / day mix.
3. **Kinetic** — walk cycle, snappier move, carry attached to the body.
4. **Involving** — wrong-part feedback, hold success pop, late-shift timer heat.
5. **No new campaign systems** — polish the verbs we already have.

## Non-goals

- New ticket kinds, bosses, or multiplayer.
- Full art overhaul / professional pixel commission (placeholders OK if clear).
- Changing pass % / sack threshold / shop economy formulas (unless a tiny speed tweak).

## Approach (chosen)

**Readable world + kinetic player + light juice** — not a content expansion.

| Pillar | Decision |
|--------|----------|
| Desk pick | Uniform random among free desks |
| Issue type | Weighted random from day-appropriate mix (still includes reboot / toner / PSU / server) |
| Type signal | Small badge/bubble **above the desk** while ticket is open (toner / PSU / reboot / server glyphs). Keep notepad as detail. |
| Edge cues | Prefer matching badge art when possible; same glyph language as desk badges |
| Movement | `move_speed` 1 → **1.5**; 4-frame walk cycle; face left/right via horizontal flip |
| Carry | Part icon follows player (above head); keep small HUD as optional backup or drop HUD if redundant |
| Hold juice | Complete: brief desk flash + existing SFX; wrong/missing part: short deny flash / stub bar + optional SFX |
| Timer | Amber under 30s, red blink under 10s |
| Closet | Highlight / alternate frame when player in interact range |

## Ticket board

See `docs/phases/phase-I.md`.

## Success feel (playtest sniff test)

After polish, Day 1 should feel like:

1. Something breaks somewhere unexpected.
2. You can tell from across the room whether you need toner, a PSU, a reboot, or the server room.
3. Running there with a bobbing cartridge feels purposeful.
4. Holding A has a little payoff; grabbing the wrong part is obvious.
5. The last 30 seconds feel tense.
