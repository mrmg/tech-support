# Tech Support — Game vision

Working title: **Tech Support** (final title optional later).

## Pitch

You’re the only tech support person in a small office. Computers break throughout the shift. You run the floor with a notepad full of tickets, reboot machines, and eventually chase parts, server faults, and printer disasters while the ticket volume climbs. Between campaign days you manage stock and budget. Neglect the floor across many days and you can get **sacked** — a run-ending game over, separate from failing a single shift and retrying.

Closest mechanical cousins: **Overcooked** / **Moving Out** — real-time movement, fetch/fix under time pressure — with a helpdesk comedy skin.

## Fantasy

- 90s / early-2000s office energy (fits GBA-era hardware fantasy).
- “Have you tried turning it off and on again?” as the tutorial mechanic.
- Escalation from trivial reboots → parts → infrastructure → org consequences (budget, reputation, sacking).

## Presentation

- **Platform:** Game Boy Advance ROM.
- **Look:** Inspired by [Sips](https://github.com/foopod/sips) — 3/4 side view with depth, slim characters, warm lo-fi pixel art. See `docs/references/`.
- **Ticket UI:** Pull-up **notepad** (Select; pauses the shift while open), not a desk-bound terminal.
- **Engine:** Butano (C++). Clean project; Sips is style reference only.

## Long-term pillars

1. **Readable chaos** — you always know what broke and roughly where.
2. **Escalating jobs** — new failure types teach new routes (storage closet, then separate rooms, etc.).
3. **Shift → day structure** — timed shifts first; then pass/fail; then multi-day campaign.
4. **Light management** — stock/budget between days (later phases).
5. **Layered consequences** — soft urgency → per-shift pass/retry → campaign sacking only when those systems exist.

## Consequences (do not conflate)

| Layer | Phase | Meaning |
|-------|-------|---------|
| Soft urgency | A+ | Visual pressure; tickets stay fixable |
| Pass / retry | B–C | End-of-shift (then end-of-day) scoregate; fail retries that shift/day |
| Sacked | G | Reputation across days; **campaign-ending** game over |

## Non-goals (for now)

- Multiplayer / link cable.
- Open-world office sim or dialogue-heavy RPG.
- Photoreal or high-res PC aesthetics.
- Exact IT certification accuracy — comedy clarity wins.
