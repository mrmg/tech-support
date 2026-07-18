# Phases

Implementation order for the Tech Support GBA game.

| Phase | Doc | Status |
|-------|-----|--------|
| A — Office & core loop (V1) | [phase-A.md](./phase-A.md) | **Complete** |
| B — Shift results | [phase-B.md](./phase-B.md) | **Complete** |
| C — Campaign days | [phase-C.md](./phase-C.md) | **Unlocked** (tickets TBD) |
| D — Carry & parts | [phase-D.md](./phase-D.md) | Planned |
| E — Multi-room office | [phase-E.md](./phase-E.md) | Planned |
| F — Stock & budget | [phase-F.md](./phase-F.md) | Planned |
| G — Reputation & sacking | [phase-G.md](./phase-G.md) | Planned |
| H — Juice & ship | [phase-H.md](./phase-H.md) | Planned |

## Sequencing

**Recommended:** A → B → C → D → E → F → G → H.

**Allowed exceptions** (also in the design spec):

| Exception | Rule |
|-----------|------|
| D before C | Carry & parts may follow B if campaign (C) is deferred |
| F without E | Stock & budget may follow D if economy only needs the same-map closet |
| G requires C | Sacking needs multi-day campaign |
| E needs carry | Multi-room fetch jobs need Phase D carry (or a minimal stub) |

Do not start a phase’s tickets until its **Unlock** line in that phase file is satisfied.

## Loop helpers

| Phase | Script | Sentinel |
|-------|--------|----------|
| A (done) | `.loop-phase-a.sh` | `AGENT_LOOP_TICK_phase_a` |
| B (done) | `.loop-phase-b.sh` | `AGENT_LOOP_TICK_phase_b` |

**How to work tickets:** [docs/dev-workflow.md](../dev-workflow.md) (`/loop` playbook).  
**Design source:** [docs/superpowers/specs/2026-07-18-tech-support-gba-design.md](../superpowers/specs/2026-07-18-tech-support-gba-design.md)
