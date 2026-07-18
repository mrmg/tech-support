# Tech Support (GBA)

Game Boy Advance office tech-support game — real-time fix-and-fetch chaos, built with **Butano**.

## Docs

| Doc | Purpose |
|-----|---------|
| [docs/game-vision.md](docs/game-vision.md) | Pitch, tone, long-term pillars |
| [docs/superpowers/specs/2026-07-18-tech-support-gba-design.md](docs/superpowers/specs/2026-07-18-tech-support-gba-design.md) | Design spec (approved) |
| [docs/superpowers/specs/2026-07-18-teaching-days-equipment-design.md](docs/superpowers/specs/2026-07-18-teaching-days-equipment-design.md) | Four-day teaching campaign and equipment-specific incidents (Phase J) |
| [docs/phases/](docs/phases/) | Phased roadmap + implementation tickets |
| [docs/known-issues.md](docs/known-issues.md) | Ship leftovers (real GBA untested, jank) |
| [docs/dev-workflow.md](docs/dev-workflow.md) | How to grind tickets with Cursor `/loop` |
| [docs/references/](docs/references/) | Sips style reference screenshots (art/UX only) |

## Status

**Phase A complete** — office & core loop (A-01…A-13).

**Phase B complete** — shift results (B-01…B-04). At the bell, every spawned ticket is classified fixed/failed; results notepad lists OK/X per ticket, completion % (`fixed / spawned`), and pass/fail vs a **75%** threshold (`shift_results::pass_threshold_percent`). Zero spawns → 100%/pass. Mid-shift: urgency stays visual-only (no fail/removal); Select notepad still pauses; hold-A still clears. Tickets: `docs/phases/phase-B.md`.

**Phase C complete** — campaign days (C-01…C-06). Linear campaign days (`campaign::max_days`; now **4** teaching days via Phase J); each day is one shift plus retries. HUD shows **Day N** (top-left) separate from the mm:ss timer. Spawn pressure rises via `campaign::day_difficulty` (table below; later days tighter gaps; per-day shift length). Brief **Day N** intro before every shift. Pass (≥75%) advances day (final-day pass → “Campaign complete” then Day 1); fail retries the same day. Title shows the session day; **A** continues without reset; **SELECT** = new game (Day 1). No SRAM — boot resets to Day 1; B→title mid-run keeps the day. Tickets: `docs/phases/phase-C.md`.

**Phase D complete** — carry & parts (D-01…D-05). One carried part (toner / PSU); Phase J replaces the old cycling cupboard with **two supply bins**. Hold-A at a desk/printer needs the matching part; reboot hold-A unchanged. Bin stock is finite (Phase F). Tickets: `docs/phases/phase-D.md`.

**Phase E complete** — multi-room office (E-01…E-05). One shift spans **office floor** + **server room** (walk into the right-wall door; return via the left-wall door; instant BG swap). Supply bins, printers, coffee, and desks stay on the office floor (hidden in server). Server rack: hold-A fills the progress bar → green LEDs; clears open `needs_server_reset` / Day 4 outage (notepad: `Server outage`). Desk hold-A cannot clear those. Edge cues point at the desk/rack/printer when same-room, else the current-room door. Tickets: `docs/phases/phase-E.md`.

**Phase F complete** — stock & budget (F-01…F-05). Session **IT budget** + bin **toner/PSU stock** (`inventory`; start **0** / **0**, budget **100**; free Day 2 toner / Day 3 PSU tutorials via J-05). Shift results earn budget (pass: `20 + %/5`; fail: `%/10`). Mid-campaign pass → between-day **shop** (toner `$15`, PSU `$25`; Up/Down, A buy, B leave) before next day intro; buys queue **pending** and arrive after `campaign::advance` (`deliver_pending`). Empty-bin A blocked; install `consume`s stock. Fail / title exit / final-day pass skip shop; campaign-complete and title SELECT `inventory::reset()`. Tickets: `docs/phases/phase-F.md`.

**Phase G complete** — reputation & sacking (G-01…G-04). Session **anger** (`reputation`, 0…100) persists across days; fail adds `12 + failed*2`, strong pass (≥90%) eases by 5. Results show `Anger N/100`; at **80+** red ink + “HR is watching” and shift HUD `HR!`. At **100** after results dismiss → distinct **SACKED** screen, then campaign/inventory/anger reset → title (skips retry/shop). Below threshold, fail still same-day retry. Resets: boot / title SELECT / campaign-complete / sacked. Tickets: `docs/phases/phase-G.md`.

**Phase H complete** — juice & ship (H-01…H-05). Placeholder music bed + key SFX; credits from title (**START**) and after final-day pass; difficulty defaults documented below; remaining jank in [`docs/known-issues.md`](docs/known-issues.md). **Real GBA untested** (mGBA only). Tickets: `docs/phases/phase-H.md`.

**Phase I complete** — fun polish (I-01…I-07). Random free desks + weighted issue mix; in-world type badges above open tickets; 4-frame walk cycle with facing and `move_speed` 1.5; carried parts bob above the player; wrong-part deny + fix pop; timer amber/red heat and closet in-range highlight. Tickets: `docs/phases/phase-I.md`.

**Phase J complete** — four short teaching days (J-01…J-10). Day 1 reboot → Day 2 printer toner → Day 3 computer PSU → Day 4 one global server outage; day-gated weights + teaching guarantees; two printers; matching toner/PSU bins (icons shared with fault + carry); zero start stock with safe Day 2/3 tutorial deliveries; optional R-sprint fatigue + coffee brew refill. Future paper-jam / software / password / cable incidents stay out of this phase. Tickets: `docs/phases/phase-J.md`.

### Difficulty defaults (J-01 / H-04)

Tunables live in headers / `campaign.cpp` — no new systems. Current defaults:

| Knob | Default | Where |
|------|---------|--------|
| Shift length | **45 / 55 / 60 / 60s** (Days 1–4) | `campaign::day_difficulty` / `shift::duration_seconds` (Day 1) |
| Campaign length | **4 days** | `campaign::max_days` |
| Pass threshold | **75%** | `shift_results::pass_threshold_percent` |
| Desk / rack hold | **2s** | `fix_interaction::reboot::hold_duration_seconds`, `server_rack::hold_duration_seconds` |
| Fail anger | **+12 + failed×2** | `reputation::anger_fail_*` |
| Strong-pass relief | **−5** at ≥90% | `reputation::anger_strong_pass_relief` |
| Warning / sack | **80** / **100** | `reputation::warning_threshold` / `sack_threshold` |
| Start stock / budget | toner **0**, PSU **0**, budget **100** (+ free Day 2 toner / Day 3 PSU tutorials) | `inventory::starting_*` / `deliver_tutorials_for_day` |
| Shop prices | toner **$15**, PSU **$25** | `inventory::toner_price` / `psu_price` |

**Spawn curve** (`campaign.cpp` `day_table` — first / interval / shrink / min / shift):

| Day | first | interval | shrink | min | shift | ≈spawns |
|-----|------:|---------:|-------:|----:|------:|--------:|
| 1 | 5 | 12 | 1 | 8 | 45 | ~4 |
| 2 | 4 | 10 | 1 | 7 | 55 | ~6 |
| 3 | 4 | 9 | 1 | 6 | 60 | ~7–8 |
| 4 | 3 | 8 | 1 | 5 | 60 | densest |

Day 1 matches `ticket::spawn::*` baseline. Short teaching shifts (Phase J); Day 4 pass completes the campaign and rolls credits.

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

## How to play

1. **Title** — **A** starts / continues the session day; **START** opens credits; **SELECT** resets to Day 1 (anger + inventory wiped).
2. **Day intro** — “Day N” plus a one-line lesson; **A** (or Start) begins the shift. A short music bed plays on title and during the shift.
3. **Shift (45–60s by day)** — Walk the office and server room (snappier walk cycle; face left/right). Hold **R** to sprint (drains energy; empty = half speed until coffee). Tickets spawn on random free desks/printers with a mixed issue mix; open targets show a type badge (reboot / toner / PSU / server). **Select** opens the notepad (pauses). Fetch parts from the toner/PSU supply bins (icon follows you); hold **A** at desks, printers, or the server rack to clear matching tickets (wrong part flashes deny); hold **A** at the coffee station to refill energy. Timer heats amber then red near the end. Edge icons point at desks, the rack, or the door when the fix is off-screen.
4. **Results** — At **0:00**, see OK/X per ticket, completion %, anger, and pass/fail (≥75%). **A** continues (retry same day or next day); **B** returns to title (day kept unless sacked).
5. **Shop** (mid-campaign pass only) — Spend IT budget on toner/PSU; **B** leaves. Orders arrive the next day.
6. **Sacked** — Anger at **100** after results → **SACKED** → title Day 1.
7. **Credits** — After a Day 4 pass (before Day 1 reset), or from title **START**. **A** / **B** / Start dismiss.

Full controls below. Leftovers and hardware notes: [`docs/known-issues.md`](docs/known-issues.md).

## Controls (Phase A–J)

| Input | Action |
|-------|--------|
| D-pad | Move (walk cycle + face left/right; door zones change rooms); shop: Up/Down select item |
| R | Sprint while held (faster walk; drains energy; at 0 energy move at half speed until coffee) |
| A | Supply bin: pick up / swap that bin’s part; desk/printer: hold to reboot or install (needs matching part; wrong part = red deny); coffee: hold to brew/refill energy; server rack: hold to reset; shop: buy; confirm title & day intro / results / sacked / credits continue |
| B | Matching supply bin: return carried part; shop: leave; results: return to title (session day kept; at sack → continue to SACKED); sacked / credits: back |
| Select | Toggle notepad during shift (pauses timer, spawns, urgency, sprint drain, coffee brew); on title = **new game** (Day 1) |
| Start | Title: **credits**; secondary confirm on day intro / credits dismiss |

### Teaching days & equipment (Phase J)

Four short shifts teach one concept each: **reboot** (Day 1), **printer toner** (Day 2), **PC PSU** (Day 3), **office-wide server outage** (Day 4). After the guaranteed teaching spawn, pools are D1 reboot 100%; D2 reboot 70% / toner 30%; D3–4 reboot 70% / toner 20% / PSU 10% (server is scripted, not random). Two printers at the office ends take toner only; PSU stays computer-only. Adjacent toner/PSU bins replace the old cycling cupboard — same `part_icon` frame for fault badge, bin, and carry. New runs start empty; Day 2/3 grant one free tutorial part (retry re-grants if the teaching install never finished). Day 4’s outage is one ticket: every PC shows the server error, computer spawns pause, cues point to the door/rack, rack hold-A clears it once. **R** sprints and drains energy; at 0 you crawl at half speed until a 1s coffee hold-A refill (optional if you never sprint).

### Fun polish (Phase I)

Desks and issue types are random within the day’s pool (not a fixed top-left / fixed cycle). Type badges float above open tickets. Carried toner/PSU rides above the player. Last 30s the timer goes amber; under 10s it blinks red. The nearest supply bin brightens when you are in pickup range.

### Audio (Phase H)

Looping bed on title + shift (`music_shift.mod`); SFX for pickup, fix complete, ticket spawn, UI open, shift end, and sacked. Placeholders — fine for ship; see known issues if channels drop a sound.

### Multi-room (Phase E)

Office right-wall door → server room; server left-wall door → office. Notepad line `Server outage` means hold-A at the rack (desk hold will not clear it). Edge icons steer you to the door when the fix is in the other room.

### Carry & parts (Phase D / J)

Walk to the adjacent toner and PSU bins (office floor only). **A** at a bin takes that part when stocked (or swaps to it when holding the other); empty/blocked **A** flashes deny. **B** returns the carried part only at its matching bin. Needs-toner (printers) / needs-PSU (desks) block hold-A until the matching part is carried; success clears the ticket, empties carry, and consumes stock. Reboot tickets still clear with hold-A alone. Carry persists across rooms.

### Reputation & sacking (Phase G)

Anger carries across days in the session. Fail worsens it; a strong pass (≥90%) eases it slightly. Results always show **Anger N/100**. At **80+**, anger turns red with “HR is watching”, and the shift HUD shows red **HR!**. Hitting **100** after results → **SACKED** (run over) → title Day 1 with anger/campaign/inventory reset. Below 100, fail is still same-day retry.

### Stock & budget (Phase F)

Passing (and failing) a shift adds IT budget. After a mid-campaign **pass**, the **IT Supply Order** shop runs before the next day intro — spend budget on toner/PSU; orders show as `+N` pending and become bin stock the next day. Zero stock blocks bin pickup for that part until a delivery or tutorial grant arrives. Fail retries the same day with no shop; final-day pass skips shop and resets economy with the campaign. Sack skips shop and wipes economy with the run reset.

### Campaign days (Phase C)

Boot → title (Day 1) → **A** → Day intro → shift. Pass advances; fail retries the same day (unless sacked). After Day 4 pass: “Campaign complete”, then credits, then **A** restarts from Day 1. **B** to title mid-campaign → **A** resumes that day (anger kept); **START** on title opens credits; **SELECT** resets to Day 1 (+ anger/inventory).

### Shift results (Phase B)

When the timer hits **0:00**, the results notepad shows **Day N**, **Anger N/100**, each spawned ticket as **OK** (fixed) or **X** (still open at the bell), `F:n O:n xx%`, and pass (“PASS - good enough”), fail (“Don't come back tomorrow”), “Anger maxed out” (at sack), or “Campaign complete” vs the 75% bar. Fail below 100 anger is flavour + same-day retry; at 100 the run ends on the **SACKED** screen after dismiss.
