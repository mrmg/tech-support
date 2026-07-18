# Dev workflow — tickets + Cursor `/loop` + sub-agents

This project is built **phase by phase**. Work is tracked as **checkbox tickets** in `docs/phases/phase-*.md`. Cursor’s **`/loop`** drives progress; **each ticket runs in a fresh sub-agent** so the long-running parent chat stays thin.

## Source of truth

| What | Where |
|------|--------|
| Full design | `docs/superpowers/specs/2026-07-18-tech-support-gba-design.md` |
| Current phase tickets | `docs/phases/phase-C.md` (A–B complete; expand C tickets when starting) |
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

## Context management (required)

Long Phase A loops will blow context if the parent chat implements every ticket. Use this split:

| Role | Responsibility |
|------|----------------|
| **Parent (loop chat)** | Pick next `- [ ]` ticket; launch **one** sub-agent; verify result; short status; wait for next tick |
| **Sub-agent (per ticket)** | Fresh context; implement **only** that ticket; `make`; update checkbox or BLOCKED note; return a short summary |

**Rules**

- **One ticket → one sub-agent → one tick.** Never stack multiple tickets in one sub-agent.
- Parent does **not** rewrite large code after the sub-agent; only unblock or fix checkbox drift.
- Sub-agent brief must be **self-contained** (ticket text + paths + design constraints). Do not rely on parent chat history.
- If blocked (e.g. needs sudo for toolchain), sub-agent leaves `- [ ]`, writes **BLOCKED**, parent stops advancing until you resolve it.

## Optimal `/loop` pattern

### What `/loop` is

`/loop` re-runs a prompt on a schedule in **this chat session** until you stop it. Combined with sub-agents, each wake is: *select ticket → spawn clean worker → summarize*.

### Recommended cadence

| Mode | When to use |
|------|-------------|
| **`/loop 15m …`** | Default for Butano work (compile + small feature) |
| **`/loop 10m …`** | Small tickets (HUD tweak, constant change) |
| **`/loop` (dynamic)** | Agent picks the next wait; good when ticket sizes vary |
| Avoid **&lt;5m** | Too chatty; builds/sub-agents won’t finish |

### Phase B note (current)

Phase A is complete. Active tickets live in `docs/phases/phase-B.md`. Locked rules: classify **only at the bell**; `% = fixed/(fixed+failed)`; pass threshold **75**; fail copy is flavour only (not Phase G sacking).

For faster mGBA checks of end screens, temporarily lower `shift::duration_seconds` in `include/shift.h`, then set it back to **120**.

### Recommended prompt (copy/paste) — Phase B

Also stored in `.loop-phase-b-prompt.json` for the shell loop helper:

```text
/loop 15m Work the next open Phase B implementation ticket using a fresh sub-agent.

Orchestrator rules (this chat stays thin):
1. Open docs/phases/phase-B.md and find the first ticket heading that is still "- [ ]" (not "- [x]").
2. Do NOT implement the ticket in this parent context. Launch exactly one Task subagent (generalPurpose) with a clean, self-contained brief for ONLY that ticket.
3. Subagent brief must include: ticket id/title/Done when/Notes; repo path; follow design spec Phase B rules; never fail tickets mid-shift; % and 75% threshold; A retry / B title; make + mgba; mark "- [x]" only if Done when met; BLOCKED note if stuck; short summary.
4. Wait for the subagent. Do not start a second ticket.
5. If no open Phase B tickets, report Phase B complete — do not start Phase C unless asked.
6. End the tick with: ticket id, what changed, build status, next open ticket id.
7. Keep parent replies short.
```

### Session startup

1. Butano + devkitARM + `mgba` already available from Phase A.
2. Skim `phase-B.md` for open tickets.
3. Prefer **stop** any leftover Phase A loop helper before starting Phase B’s.
4. Keep this chat available so loop ticks and sub-agents can finish.

### Helper scripts

| File | Purpose |
|------|---------|
| `.loop-phase-b.sh` | Sleeps 15m, prints `AGENT_LOOP_TICK_phase_b` + prompt JSON |
| `.loop-phase-b-prompt.json` | Phase B prompt payload |
| `.loop-phase-a.sh` / `.loop-phase-a-prompt.json` | Phase A (complete — do not use for new work) |

Stop with: ask the agent to **stop the loop** (kills the helper PID).

## Alternatives

| Approach | Use when |
|----------|----------|
| Normal chat + one sub-agent | Single ticket without a schedule |
| Cloud / background agent | AFK on a branch; still prefer one ticket per agent |
| Cursor Automations | Recurring schedule outside interactive chat |

## Definition of a good tick

1. Exactly one ticket advanced (or explicit BLOCKED).
2. Work done in a **fresh sub-agent**, not the bloated parent.
3. Project still builds (or BLOCKED explains why not).
4. `phase-*.md` checkbox matches reality.
5. No drive-by refactors outside that ticket.

## Pairing with git

- **After each successful loop tick:** the parent creates **one logical commit** for that ticket (code + `phase-*.md` checkbox). Message focuses on why; HEREDOC commit message.
- Do **not** commit on BLOCKED ticks.
- Do **not** push unless the user explicitly asks.
- Good grain: one commit per ticket. Keep checkbox updates with the code.
