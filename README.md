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

**Phase A complete** — office & core loop (A-01…A-13): title → timed shift → tickets → notepad → world cues → hold-to-reboot → soft urgency → shift-end summary + retry.

**Next:** Phase B (see `docs/phases/`).

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

## Controls (Phase A)

| Input | Action |
|-------|--------|
| D-pad | Move |
| A | Hold to reboot (in desk range) / confirm title / retry shift (end screen) |
| B | Return to title (end screen) |
| Select | Toggle notepad (pauses shift timer, spawns, and urgency while open) |
| Start | Reserved (secondary confirm on title only) |
