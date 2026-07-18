# Dev workflow — tickets + Cursor `/loop`

This project is built **phase by phase**. Work is tracked as **checkbox tickets** in `docs/phases/phase-*.md`. Cursor’s **`/loop`** is the best in-chat way to grind those tickets without re-explaining context every time.

## Source of truth

| What | Where |
|------|--------|
| Full design | `docs/superpowers/specs/2026-07-18-tech-support-gba-design.md` |
| Current phase tickets | `docs/phases/phase-A.md` (then B, C, …) |
| Vision / tone | `docs/game-vision.md` |

Only mark a ticket `- [x]` when its **Done when** criteria are met (ROM builds / behaviour visible in mGBA).

## Ticket checkbox format

```markdown
### - [ ] A-03 — Short title

- **Done when:** emulator-checkable criteria
- **Notes:** optional
```

Completed:

```markdown
### - [x] A-03 — Short title
```

## Optimal `/loop` pattern

### What `/loop` is

`/loop` re-runs a prompt on a schedule in **this chat session** until you stop it. It is not a separate product backlog; it drives the agent against the ticket files in the repo.

### Recommended cadence

| Mode | When to use |
|------|-------------|
| **`/loop 15m …`** | Default for Butano work (compile + small feature) |
| **`/loop 10m …`** | Small tickets (HUD tweak, constant change) |
| **`/loop` (dynamic)** | Agent picks the next wait; good when ticket sizes vary |
| Avoid **&lt;5m** | Too chatty; builds won’t finish; context thrash |

**One ticket per tick.** Do not ask the loop to “finish all of Phase A” in one go — that burns context and skips verification.

### Scaffold first, then loop

1. Complete **A-01** (Butano scaffold) in a **normal chat** — toolchain setup is a poor fit for a timed loop.
2. Confirm `make` produces a `.gba` that runs in mGBA.
3. Start `/loop` from the first remaining open ticket (usually **A-02**).

### Recommended prompt (copy/paste)

Use **after A-01 is done** (or change “A-02” if you intentionally loop a later ticket):

```text
/loop 15m Work the next open Phase A implementation ticket.

Rules:
1. Open docs/phases/phase-A.md and find the first ticket heading that is still "- [ ]" (not "- [x]").
2. Implement ONLY that ticket. Do not start the next one.
3. Follow docs/superpowers/specs/2026-07-18-tech-support-gba-design.md. Sips is art/UX reference only — do not fork Sips.
4. Respect locked controls: A = hold-to-fix / confirm; Select = notepad; Start reserved.
5. Build the ROM (make). Fix compile errors before finishing the tick.
6. If you can only verify via build (not mGBA), note exact human check steps under that ticket's Notes.
7. Mark the ticket heading "- [x]" only when Done when is satisfied. Do not check acceptance-criteria boxes unless you verified them.
8. If blocked (tooling missing, ambiguous design), leave the ticket "- [ ]", add a short BLOCKED note under Notes, and stop advancing tickets this tick.
9. End the tick with: ticket id, what changed, build status, next open ticket id.
```

For a later phase, swap `phase-A.md` / “Phase A” for the active phase file.

### Session startup (before first `/loop`)

1. Confirm Butano + devkitARM are installed and `make` produces a `.gba`.
2. Skim the active `phase-*.md` — know what’s `- [x]` vs `- [ ]`.
3. Keep the Agents / chat window available so loop ticks can run.

### During the loop

- Prefer leaving the machine able to compile (don’t hold exclusive locks on the tree in another half-finished experiment).
- If a tick only needs a human mGBA check, do that check before related tickets are marked done.
- Stop the loop when: phase complete, you’re done for the day, or a **BLOCKED** note needs a design answer.

### Stopping

Tell the agent to **stop the loop** (or stop `/loop`). Do not leave a blocked ticket checked.

## Alternatives (when not to use `/loop`)

| Approach | Use when |
|----------|----------|
| Normal chat (“do ticket A-04”) | Exploratory work, art direction, **A-01 scaffold**, first-time toolchain |
| Cloud / background agent | Progress on a branch while AFK; give it 1–3 ticket IDs max |
| Cursor Automations | Recurring schedule outside an interactive session |
| Multi-ticket PR by hand | Refactors that touch many tickets at once (update checkboxes after) |

## Definition of a good tick

A tick is successful if **all** of these are true:

1. Exactly one ticket advanced (or an explicit BLOCKED note).
2. Project still builds.
3. `phase-*.md` checkbox state matches reality.
4. No drive-by refactors outside that ticket’s scope.

## Pairing with git

- Commit when **you** ask (or at natural phase milestones) — don’t rely on the loop to invent commit policy.
- Good commit grain: one commit per ticket, or one commit per small group of related tickets after human review.
- Keep `docs/phases/*.md` checkbox updates in the same commit as the code when possible.
