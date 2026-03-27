# morse-trainer

Embedded C++ firmware for a **Raspberry Pi Pico** running on a **Pimoroni Pico Display Pack** (ST7789 240×135 display, 4 buttons, RGB LED). A multi-mode handheld device with:

- **Morse Trainer** — practice sending Morse code with live feedback and clue system
- **Resistor Calculator** — build 4- or 5-band resistor color codes
- **Settings** — configurable timing, brightness, practice set, idle timeout
- **Idle screensaver** — animated rain/lightning effect, auto-activates on inactivity
- **Help / About** — on-device documentation and build info

## Hardware

| Component | Details |
|-----------|---------|
| MCU | RP2040 (Raspberry Pi Pico) |
| Display | ST7789, 240×135, SPI |
| Buttons | A, B, X, Y |
| LED | RGB (Pimoroni RGBLED) |
| Board | Pimoroni Pico Display Pack |

## Prerequisites

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) cloned at `../pico-sdk`
- [Pimoroni Pico libraries](https://github.com/pimoroni/pimoroni-pico) cloned at `../pimoroni-pico`
- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) installed at `/Applications/ArmGNUToolchain/15.2.rel1/`
- CMake ≥ 3.12, Ninja

Both SDKs must be clean clones — do not let an IDE write into them.

## Building

```bash
# Configure (one-time, or after CMakeLists changes)
cmake --preset debug
cmake --preset release

# Build
cmake --build --preset debug
cmake --build --preset release

# Build and flash (Pico already mounted via picotool)
cmake --build --preset debug && picotool load build/debug/src/morse-trainer.uf2 --force --update
```

Build artifacts land in `build/debug/src/` or `build/release/src/`:
`morse-trainer.uf2`, `.elf`, `.bin`, `.hex`, `.map`

To flash manually: hold BOOTSEL, connect USB, drag-and-drop the `.uf2` onto the mounted drive.

## Project Structure

```
src/
  morse_trainer.cpp       # main() — constructs App, runs 60 fps loop
  app.h / app.cpp         # owns all screens, drives lifecycle, auto-idle timer
  screen.h                # Screen base class + ScreenId enum + SwitchFn
  button-handler.h/.cpp   # polling-based button handler (DOWN / SHORT / LONG)
  settings.h              # Settings struct (POD, flash-persisted)
  settings_store.h/.cpp   # flash persistence (last sector, erase+program)
  screens/
    menu_screen.*         # top-level menu
    morse_screen.*        # Morse trainer
    resistor_screen.*     # resistor band calculator
    help_screen.*         # scrollable help pages
    about_screen.*        # build info (git SHA, date, version)
    settings_screen.*     # settings editor
    idle_screen.*         # rain/lightning screensaver
cmake/
  gen_version.cmake       # generates version_generated.h from git + date
CMakePresets.json         # debug / release presets
```

## Button Layout

| Screen | A | A-Long | B | X | Y |
|--------|---|--------|---|---|---|
| Menu | Select | Back to menu | — | Up | Down |
| Morse Trainer | — | Back to menu | Reset input / Toggle clue | Next trial / Toggle display | dit (.) / dah (−) |
| Resistor | Band left | Back to menu | Band right | Value up / Toggle bands | Value down |
| Help | Select / Back | Back to menu | Back | Prev page | Next page |
| Settings | Increase | Back to menu | Decrease / Save+back | Prev item | Next item |
| Idle | Restart | Back to menu | Resume | — | — |

A-Long always returns to the main menu from any screen.
X-Long on the Idle screen toggles the lightning LED effect.

## Settings

| Setting | Default | Range |
|---------|---------|-------|
| dit/dah threshold | 250 ms | 50–500, step 10 |
| Idle timeout | 30 s | 0 (disabled) – unlimited, step 10 |
| Practice set | A–Z | A–Z or A–Z + 0–9 |
| Brightness | 175 | 0–255, step 5 |
| Clue after N wrong | 5 | 1–10 |

Settings are persisted to the last flash sector and survive reboots. If the magic number doesn't match (layout changed), defaults are loaded.

## Architecture Notes

- **Main loop**: `buttons.poll()` → `app.update()` → `st7789.update()` at ~60 fps; no IRQs used for buttons
- **Screen lifecycle**: `on_enter()` called on activation; `update()` called every frame; `on_button()` forwarded from ButtonHandler
- **Auto-idle**: `App` tracks `last_activity_us_`; on timeout saves `pre_idle_` screen, switches to `IdleScreen`; `ScreenId::RESUME` restores it without calling `on_enter()`
- **Fonts**: `font6` for body text; Hershey `"sans"` (thickness 2) for large scaled text — always reset to `font6` + thickness 1 after
- **Namespace**: all custom code is in `troublemaker::`
- **Compiler**: `-Wall -Werror` — all warnings are errors

## Build Metadata

`APP_GIT_SHA` and `APP_BUILD_DATE` are generated into `version_generated.h` by `cmake/gen_version.cmake` on every build after a commit (triggered by `DEPENDS .git/index`). No reconfigure needed. Displayed on the About screen.
