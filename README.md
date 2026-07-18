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

**Phase B complete** — shift results (B-01…B-04). At the bell, every spawned ticket is classified fixed/failed; results notepad lists OK/X per ticket, completion % (`fixed / spawned`), and pass/fail vs a **75%** threshold (`shift_results::pass_threshold_percent`). Zero spawns → 100%/pass. Mid-shift: urgency stays visual-only (no fail/removal); Select notepad still pauses; hold-A still clears. **A** retries the shift; **B** returns to title. Tickets: `docs/phases/phase-B.md`.

**Phase C next** — campaign days. Tickets: `docs/phases/phase-C.md` (expand when starting).

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

## Controls (Phase A–B)

| Input | Action |
|-------|--------|
| D-pad | Move |
| A | Hold to reboot (in desk range) / confirm title / retry shift (results) |
| B | Return to title (results) |
| Select | Toggle notepad (pauses shift timer, spawns, and urgency while open) |
| Start | Reserved (secondary confirm on title only) |

### Shift results (Phase B)

When the timer hits **0:00**, the results notepad shows each spawned ticket as **OK** (fixed) or **X** (still open at the bell), `F:n O:n xx%`, and pass (“PASS - good enough”) or fail (“Don't come back tomorrow”) vs the 75% bar. Fail is flavour + retry — not campaign sacking.
