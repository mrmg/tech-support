# Dev workflow — tickets + Cursor `/loop`

This project is built **phase by phase**. Work is tracked as checkboxes in `docs/phases/phase-*.md`. Cursor’s **`/loop`** is the best in-chat way to grind those tickets without re-explaining context every time.

## Source of truth

| What | Where |
|------|--------|
| Full design | `docs/superpowers/specs/2026-07-18-tech-support-gba-design.md` |
| Current phase tickets | `docs/phases/phase-A.md` (then B, C, …) |
| Vision / tone | `docs/game-vision.md` |

Only mark a ticket `[x]` when its **Done when** criteria are met (ROM builds / behaviour visible in mGBA).

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

### Recommended prompt (copy/paste)

Use this after Phase A tickets exist and the Butano project is scaffolded:

```text
/loop 15m Work the next open Phase A implementation ticket.

Rules:
1. Open docs/phases/phase-A.md and find the first unchecked ticket (- [ ]).
2. Implement ONLY that ticket. Do not start the next one.
3. Follow docs/superpowers/specs/2026-07-18-tech-support-gba-design.md and keep Sips as art/UX reference only (do not fork Sips).
4. Build the ROM (make). Fix compile errors before finishing the tick.
5. If you can verify in emulator/headless build only, note what a human should click in mGBA under the ticket Notes.
6. Mark the ticket [x] in phase-A.md only when Done when is satisfied.
7. If blocked (needs art decision, tooling missing, ambiguous design), stop the loop work for this tick, leave the ticket unchecked, and write a short BLOCKED note under that ticket.
8. End the tick with: ticket id, what changed, build status, next open ticket id.
```

For a later phase, swap `phase-A.md` / “Phase A” for the active phase file.

### Session startup (before first `/loop`)

1. Confirm Butano + devkitARM are installed and `make` produces a `.gba`.
2. Skim the active `phase-*.md` — know what’s done.
3. Run **one ticket manually** in chat if the scaffold is new, then start `/loop` for the rest.
4. Keep the Agents / chat window available so loop ticks can run.

### During the loop

- Prefer leaving the machine able to compile (don’t hold exclusive locks on the tree in another half-finished experiment).
- If a tick only needs a human mGBA check, do that check before the next tick marks related tickets done.
- Stop the loop when: phase complete, you’re done for the day, or a **BLOCKED** note needs a design answer.

### Stopping

Tell the agent to **stop the loop** (or stop `/loop`). Do not leave a blocked ticket checked.

## Alternatives (when not to use `/loop`)

| Approach | Use when |
|----------|----------|
| Normal chat (“do ticket A-04”) | Exploratory work, art direction, first scaffold |
| Cloud / background agent | You want progress on a branch while AFK; give it 1–3 ticket IDs max |
| Cursor Automations | Recurring schedule (e.g. daily “next open ticket”) outside an interactive session |
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
