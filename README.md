# Tech Support (GBA)

Game Boy Advance office tech-support game — real-time fix-and-fetch chaos, built with **Butano**.

## Docs

| Doc | Purpose |
|-----|---------|
| [docs/game-vision.md](docs/game-vision.md) | Pitch, tone, long-term pillars |
| [docs/superpowers/specs/2026-07-18-tech-support-gba-design.md](docs/superpowers/specs/2026-07-18-tech-support-gba-design.md) | Design spec (approved) |
| [docs/phases/](docs/phases/) | Phased roadmap + implementation tickets |
| [docs/dev-workflow.md](docs/dev-workflow.md) | How to grind tickets with Cursor `/loop` |
| [docs/references/](docs/references/) | Sips style reference screenshots (art/UX only) |

## Status

**Phase A complete** — office & core loop (A-01…A-13).

**Phase B complete** — shift results (B-01…B-04). At the bell, every spawned ticket is classified fixed/failed; results notepad lists OK/X per ticket, completion % (`fixed / spawned`), and pass/fail vs a **75%** threshold (`shift_results::pass_threshold_percent`). Zero spawns → 100%/pass. Mid-shift: urgency stays visual-only (no fail/removal); Select notepad still pauses; hold-A still clears. Tickets: `docs/phases/phase-B.md`.

**Phase C complete** — campaign days (C-01…C-06). Linear **Day 1…5** (`campaign::max_days`); each day is one shift plus retries. HUD shows **Day N** (top-left) separate from the mm:ss timer. Spawn pressure rises via `campaign::day_difficulty` (Day 1 = Phase A baseline; later days tighter gaps; shift length still 120s). Brief **Day N** intro before every shift. Pass (≥75%) advances day (final-day pass → “Campaign complete” then Day 1); fail retries the same day. Title shows the session day; **A** continues without reset; **SELECT** = new game (Day 1). No SRAM — boot resets to Day 1; B→title mid-run keeps the day. Tickets: `docs/phases/phase-C.md`.

**Phase D complete** — carry & parts (D-01…D-05). Same-map **storage closet** (aisle cupboard); **one** carried part (toner / PSU). At closet: **A** pick up or **replace**-swap toner↔PSU; **B** return part. HUD `part_icon` bottom-right. Spawner mixes `reboot` / `needs_toner` / `needs_psu`; notepad shows the issue line. Needs-part: hold-A at desk only progresses with the matching part, then consumes carry; reboot hold-A unchanged. Infinite closet stock (Phase F will add ordering). Tickets: `docs/phases/phase-D.md`.

**Phase E next** — multi-room office (unlocked). Tickets: `docs/phases/phase-E.md`.

## Toolchain setup (macOS)

1. **devkitPro** — install the pacman package from [devkitPro Getting Started](https://devkitpro.org/wiki/Getting_Started), then install the GBA toolchain group:

   ```bash
   sudo dkp-pacman -S gba-dev
   ```

2. **Environment** (add to `~/.zshrc` or export in the shell before building):

   ```bash
   export DEVKITPRO=/opt/devkitpro
   export DEVKITARM=/opt/devkitpro/devkitARM
   export PATH="$DEVKITPRO/tools/bin:$DEVKITARM/bin:$PATH"
   ```

3. **Python 3** — required by Butano’s asset pipeline (`python3` on PATH).

4. **Butano** — vendored as a git submodule at `third_party/butano` (release **21.7.0**):

   ```bash
   git submodule update --init --recursive
   ```

5. **mGBA** — on PATH as `mgba` (Homebrew or Nix). Examples:

   ```bash
   brew install mgba
   # or: nix profile install nixpkgs#mgba
   ```

### `LIBBUTANO`

The Makefile points at the Butano library directory:

```text
LIBBUTANO := third_party/butano/butano
```

That path is relative to the repo root (where the Makefile lives). Do not put Butano in a path with spaces.

## Build & run

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export PATH="$DEVKITPRO/tools/bin:$DEVKITARM/bin:$PATH"

make -j$(sysctl -n hw.ncpu)
mgba tech-support.gba
```

Output ROM: `tech-support.gba` (title `TECHSUPPORT`, code `TS01`).

```bash
make clean   # remove build/ and the ROM
```

## Controls (Phase A–D)

| Input | Action |
|-------|--------|
| D-pad | Move |
| A | Closet: pick up / swap part; desk: hold to reboot or install (needs matching part); confirm title & day intro / results continue |
| B | Closet: return carried part; results: return to title (session day kept) |
| Select | Toggle notepad during shift (pauses timer, spawns, urgency); on title = **new game** (Day 1) |
| Start | Secondary confirm on title / day intro |

### Carry & parts (Phase D)

Walk to the aisle cupboard. **A** takes toner when empty, or swaps toner↔PSU when holding. **B** clears the slot. Needs-toner / needs-PSU tickets block hold-A until the matching part is carried; success clears the ticket and empties carry. Reboot tickets still clear with hold-A alone.

### Campaign days (Phase C)

Boot → title (Day 1) → **A** → Day intro → shift. Pass advances; fail retries the same day. After Day 5 pass: “Campaign complete”, then **A** restarts from Day 1. **B** to title mid-campaign → **A** resumes that day; **SELECT** on title resets to Day 1.

### Shift results (Phase B)

When the timer hits **0:00**, the results notepad shows **Day N**, each spawned ticket as **OK** (fixed) or **X** (still open at the bell), `F:n O:n xx%`, and pass (“PASS - good enough”), fail (“Don't come back tomorrow”), or “Campaign complete” vs the 75% bar. Fail is flavour + same-day retry — not Phase G sacking.
