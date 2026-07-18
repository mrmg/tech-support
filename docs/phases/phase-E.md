# Phase E — Multi-room office

**Unlock:** Phase D complete. **Active.**  
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

- [ ] At least two rooms reachable in one shift
- [ ] Tickets can require another room
- [ ] Off-screen cues remain fair across rooms

## Tickets

### - [x] E-01 — Second room + transition

- **Done when:** Player can leave the main office through a door/transition into a second room map and return.
- **Notes:** Simple fade or instant transition OK. Separate BG for room 2.
- **mGBA:** Walk into door → other room; return to office.

### - [ ] E-02 — Other-room interactable

- **Done when:** Second room has a usable interactable (server rack or printer) with hold-A or press-A action that can satisfy a ticket step.
- **Notes:** Placeholder art OK.
- **mGBA:** Interact in room 2 does something visible.

### - [ ] E-03 — Cross-room ticket type

- **Done when:** At least one ticket type requires visiting the other room (e.g. reset server, then optionally return). Spawner can emit it; notepad explains.
- **Notes:** Can combine with parts or be standalone “network down → server room”.
- **mGBA:** Ticket points to other room; completing interact clears or advances ticket.

### - [ ] E-04 — Cues across rooms

- **Done when:** If the ticket target is in another room, edge indicators still guide the player (toward door or generic “other room” cue).
- **Notes:** Fair, not cryptic.
- **mGBA:** Leave office with open cross-room ticket → cue toward exit/door.

### - [ ] E-05 — Phase E pass (integration)

- **Done when:** Acceptance criteria met; E-01…E-04 `- [x]`; README updated; jank for F listed.
- **mGBA:** Full path office ↔ room 2 with a cross-room ticket.
