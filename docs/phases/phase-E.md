# Phase E — Multi-room office

**Unlock:** Phase D complete. **Complete.**  
**Does not require Phase F.**

## Goal

At least two rooms in one shift (office floor + server or printer room). Transitions between them. Some tickets need the other room. Off-screen cues stay fair across rooms.

## In scope

- Room transition (door / edge)
- Second room with interactable (server rack and/or printer)
- Ticket kind that requires other-room action
- Cues work when target is in another room

## Out of scope

- Budget shop (F)
- Removing Phase D closet (may stay on main floor)

## Acceptance criteria

- [x] At least two rooms reachable in one shift
- [x] Tickets can require another room
- [x] Off-screen cues remain fair across rooms

## Tickets

### - [x] E-01 — Second room + transition

- **Done when:** Player can leave the main office through a door/transition into a second room map and return.
- **Notes:** Simple fade or instant transition OK. Separate BG for room 2.
- **mGBA:** Walk into door → other room; return to office.

### - [x] E-02 — Other-room interactable

- **Done when:** Second room has a usable interactable (server rack or printer) with hold-A or press-A action that can satisfy a ticket step.
- **Notes:** Placeholder art OK. Hold-A at rack fills progress bar → green LEDs; stubs `server_reset_done` for the shift (E-03 will clear tickets from it).
- **mGBA:** Interact in room 2 does something visible.
- **Verification:** `server_rack` entity on server map (solid + sprite); hold-A in range uses shared progress bar; complete sets `server_reset_done` and swaps amber/red LEDs → green. Hidden on office floor; room solids include rack AABB.

### - [x] E-03 — Cross-room ticket type

- **Done when:** At least one ticket type requires visiting the other room (e.g. reset server, then optionally return). Spawner can emit it; notepad explains.
- **Notes:** Can combine with parts or be standalone “network down → server room”.
- **mGBA:** Ticket points to other room; completing interact clears or advances ticket.
- **Verification:** `ticket::type::needs_server_reset` in spawn rotation; notepad label `reset server`; desk hold-A skips it; rack hold complete → `clear_server_reset_tickets` (history Fixed); rack re-arms if another of this kind spawns after a prior reset.

### - [x] E-04 — Cues across rooms

- **Done when:** If the ticket target is in another room, edge indicators still guide the player (toward door or generic “other room” cue).
- **Notes:** Fair, not cryptic. `world_cues` resolves cue target by room + ticket kind: same-room desk/rack, else current-room door (office→server door for `needs_server_reset`; server→office door for desk tickets). No pathfinding.
- **mGBA:** Leave office with open cross-room ticket → cue toward exit/door.
- **Verification:** Office + open `needs_server_reset` → edge icon toward right door; in server with open desk ticket → edge icon toward left return door; same-room cues unchanged (desk / rack).

### - [x] E-05 — Phase E pass (integration)

- **Done when:** Acceptance criteria met; E-01…E-04 `- [x]`; README updated; jank for F listed.
- **Notes:**
  - **Verification:** E-01…E-04 already `- [x]`. Acceptance criteria confirmed by code-path review: `room::id` office↔server via door zones + enter spawns; instant BG/solids swap; closet + desks office-only (hidden in server); `server_rack` hold-A → green LEDs + `clear_server_reset_tickets`; `needs_server_reset` in spawn mix + notepad `reset server`; desk hold-A skips server-reset kinds; `world_cues` door/rack/desk targets by room; Phase D carry/closet still wired in `shift_scene` (office branch); shift bell → results → campaign day advance unchanged.
  - **Build:** `DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM make -j…` → `tech-support.gba` (title `TECHSUPPORT` / `TS01`).
  - **Human mGBA path:** `mgba tech-support.gba`. Walk office right door ↔ server left door; wait for `reset server` ticket; hold-A at rack; confirm notepad/results. Closet A/B + needs-part still on office floor.
  - **Known jank for Phase F:** rack hold-A works even with no open `needs_server_reset` (sets green until a later ticket re-arms); one rack clear fixes **all** open server-reset tickets at once; cross-room edge icons can stack on the same door when several tickets point there; desk with `reset server` still shows broken PC (hold-A silent — must use rack); instant room swap (no fade); A overloaded further (closet / desk / rack / confirms); wrong/missing part still silent; infinite closet stock (F territory); placeholder server-room / rack art; carry HUD may sit oddly across rooms; Phase C leftovers remain (120s days, no SRAM, day intro bare, OK/X results, 4-row cap, A/SELECT discoverability).
- **mGBA:** Full path office ↔ room 2 with a cross-room ticket.
