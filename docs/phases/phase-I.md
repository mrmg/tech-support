# Phase I — Fun polish (readability, motion, juice)

**Unlock:** Phase H complete.  
**Design:** [docs/superpowers/specs/2026-07-18-fun-polish-design.md](../superpowers/specs/2026-07-18-fun-polish-design.md)

## Goal

Make the existing loop feel faster, clearer, and more involving — random desks, typed world badges, walk anim, carry-on-body, hold/timer juice.

## Acceptance criteria

- [x] Free desks and issue types are not a fixed top-left / fixed cycle
- [x] Open tickets show an in-world type badge (toner / PSU / reboot / server)
- [x] Player has a walk cycle, faces movement direction, and moves snappier than 1 px/frame
- [x] Carried parts are visible on the player
- [x] Wrong-part and fix-complete give clear feedback; late timer heats up

## Tickets

### - [x] I-01 — Random free desk + issue mix

- **Done when:** Spawn picks a uniform random free desk; issue type is weighted/random from the allowed mix (not `% 5` cycle). First ticket of a shift is not always desk 0.
- **Notes:** Use Butano RNG (`bn::random` or existing pattern). Keep Day difficulty spawn *timing* from campaign.
- **Implemented:** Shared `bn::random` in `ticket.cpp`; `_pick_free_desk` uniform among free desks; `_pick_issue_type` weighted 2:1:1:1 (reboot/toner/server/PSU). `spawn::tick_rng()` on title + day-intro waits. Spawn *timing* still from `campaign::day_difficulty` / `spawn::params`.

### - [x] I-02 — Desk type badges

- **Done when:** Open ticket shows a small badge/bubble above that desk: reboot / toner / PSU / server. Distinct enough to read without notepad.
- **Notes:** Placeholder art OK. Sync clear when ticket clears. Server-reset tickets on office desks still show server badge (player must leave room).
- **Implemented:** `graphics/ticket_badge.bmp` 4 frames (reboot / toner / PSU / server). `desk::entity` optional badge sprite above desk; `shift_scene` syncs with broken state via `issue_type_for_desk`; cleared on ticket clear; server-reset keeps server frame on the office desk.

### - [x] I-03 — Walk cycle + facing + speed

- **Done when:** Player uses ≥3 walk frames while moving, idle when stopped, flips to face left/right, `move_speed` ≈ 1.5.
- **Notes:** Expand `graphics/player.bmp` strip; keep 16×16 footprint if possible.

### - [x] I-04 — Carry on player

- **Done when:** Held toner/PSU icon follows the player (above head or in hands). Corner HUD optional.
- **Notes:** Wire in `carry.cpp` / shift update.
- **Implemented:** `carry::slot::update_follow` places `part_icon` above the player each frame (slight lut_sin bob); camera-attached. Corner HUD removed as redundant.

### - [x] I-05 — Hold feedback (wrong part + complete)

- **Done when:** Wrong/missing part on hold-A gives an obvious deny (flash/stub); successful fix pops (desk flash / bar bounce) plus existing SFX.
- **Out:** New systems.
- **Implemented:** Wrong/missing part shows empty progress bar with red fade (unique palette). On complete: existing `sfx_fix_complete`, desk invert flash (~14f), full bar scale bounce.

### - [x] I-06 — Timer heat + closet highlight

- **Done when:** Timer turns amber &lt;30s and red blink &lt;10s; closet shows in-range highlight (extra frame or palette).
- **Notes:** Small polish; ship with I-05 if cheap.
- **Implemented:** `redraw_timer_hud` amber palette &lt;30s, red blink &lt;10s (`timer_blink_half_period`). Closet `set_in_range` brighter palette when player in interact zone (same range as carry pickup); wired from office branch in `shift_scene`.

### - [x] I-07 — Phase I pass

- **Done when:** Acceptance criteria met; I-01…I-06 `[x]`; README notes fun-polish controls/feel; clean `make` succeeds.
- **Verified:** I-01…I-06 wired (RNG desk/issue, badges, walk 1.5 + 4 frames + flip, carry follow, hold deny/pop, timer heat + closet highlight). Clean `make` OK. README + phases table note Phase I complete.
