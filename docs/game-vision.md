# Tech Support — Game vision

Working title: **Tech Support** (TBD).

## Pitch

You’re the only tech support person in a small office. Computers break throughout the day. You run the floor with a notepad full of tickets, reboot machines, and eventually chase parts, server faults, and printer disasters while the ticket volume climbs. Between days you manage stock and budget. Neglect the floor too long and you don’t get asked back.

Closest mechanical cousins: **Overcooked** / **Moving Out** — real-time movement, fetch/fix under time pressure — with a helpdesk comedy skin.

## Fantasy

- 90s / early-2000s office energy (fits GBA-era hardware fantasy).
- “Have you tried turning it off and on again?” as the tutorial mechanic.
- Escalation from trivial reboots → parts → infrastructure → org consequences (budget, reputation, sacking).

## Presentation

- **Platform:** Game Boy Advance ROM.
- **Look:** Inspired by [Sips](https://github.com/foopod/sips) — 3/4 side view with depth, slim characters, warm lo-fi pixel art. See `docs/references/`.
- **Ticket UI:** Pull-up **notepad** (always available), not a desk-bound terminal.
- **Engine:** Butano (C++). Clean project; Sips is style reference only.

## Long-term pillars

1. **Readable chaos** — you always know what broke and roughly where.
2. **Escalating jobs** — new failure types teach new routes (storage, server room, etc.).
3. **Day structure** — timed shifts; later, pass/fail and multi-day campaign.
4. **Light management** — stock/budget between days (later phases).
5. **Consequences** — soft urgency first; sacking only when the systems exist to support it.

## Non-goals (for now)

- Multiplayer / link cable.
- Open-world office sim or dialogue-heavy RPG.
- Photoreal or high-res PC aesthetics.
- Exact IT certification accuracy — comedy clarity wins.
