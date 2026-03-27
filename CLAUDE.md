# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Embedded C++ firmware for a Raspberry Pi Pico running on a Pimoroni Pico Display Pack (ST7789 240×135 display + 4 buttons + RGB LED). The project is a multi-mode device: Morse Trainer, Resistor Calculator, Help, About, Settings, and an auto-activating rain/lightning Idle screensaver.

## Build System

CMake cross-compilation project targeting the RP2040, using `CMakePresets.json`. Both SDKs are siblings of this repo:

- **Pico SDK**: `../pico-sdk`
- **Pimoroni Pico libraries**: `../pimoroni-pico` — auto-detected as sibling of the Pico SDK

```bash
# Configure
cmake --preset debug
cmake --preset release

# Build
cmake --build --preset debug
cmake --build --preset release

# Build and flash in one step (Pico must already be mounted)
cmake --build --preset debug && picotool load build/debug/src/morse-trainer.uf2 --force --update

# Flash: drag-and-drop build/debug/src/morse-trainer.uf2 onto the Pico in BOOTSEL mode
```

Build artifacts (`morse-trainer.uf2`, `.elf`, `.bin`, `.hex`, `.map`) land in `build/debug/src/` or `build/release/src/`.

Compiler flags include `-Wall -Werror` — all warnings are errors.

### Build-time Metadata

Static strings (`APP_NAME`, `APP_AUTHOR`, `APP_VERSION`) are injected via `target_compile_definitions`.

`APP_GIT_SHA` and `APP_BUILD_DATE` are written into `version_generated.h` by `cmake/gen_version.cmake`, which runs as an `add_custom_command(OUTPUT)` with `DEPENDS .git/index`. This means every `cmake --build` after a commit regenerates the header and recompiles `about_screen.cpp` in the correct order — no reconfigure needed. `about_screen.cpp` includes `version_generated.h` directly.

## Architecture

### Entry Point & Main Loop

`src/morse_trainer.cpp` contains `main()`. It constructs `App`, `ButtonHandler`, then runs the main loop at ~60 fps:

```cpp
while (true) {
    buttons.poll();
    app.update();
    st7789.update(&graphics);
    sleep_ms(1000 / 60);
}
```

### App Class

`src/app.h` / `src/app.cpp` — owns all screen instances and drives the screen lifecycle:

- `on_button(ButtonId, PressType)` — forwarded from ButtonHandler; resets `last_activity_us_`; A-Long always returns to menu
- `update()` — checks auto-idle timer, delegates to `active_->update()`
- `set_screen(ScreenId)` — activates a screen and calls `on_enter()`; handles `ScreenId::RESUME` specially (see below)

### Screen System

`src/screen.h` defines the `Screen` base class and `ScreenId` enum:

```cpp
enum class ScreenId { MENU, IDLE, MORSE_TRAINER, RESISTOR_CALCULATOR, HELP, ABOUT, SETTINGS, RESUME };
```

Each screen implements `on_enter()`, `update()`, and `on_button()`. Screens navigate via a `SwitchFn` callback injected at construction.

### Auto-Idle Screensaver

- `App::update()` compares `time_us_64() - last_activity_us_` against `settings_.idle_timeout_s`
- When the timer expires, `pre_idle_ = active_` is saved, then `set_screen(IDLE)` is called
- `ScreenId::RESUME`: `set_screen()` restores `active_ = pre_idle_` **without** calling `on_enter()`, preserving all screen state; falls back to MENU if `pre_idle_` is null
- Any explicit non-IDLE navigation clears `pre_idle_`

### Display & Graphics

Uses `PicoGraphics_PenRGB332` rendered through `ST7789`. All drawing happens each frame via `active_->update()`, then flushed with `st7789.update(&graphics)`.

**Fonts**: bitmap `font6` (default) for most text; hershey `"sans"` with `set_thickness(2)` for large/scaled text (e.g. resistance value in ResistorScreen). Always reset to `font6` + `thickness(1)` after using hershey.

### Button Handling

Four buttons: **A**, **B**, **X**, **Y** (mapped to `PicoDisplay::A/B/X/Y`).

`src/button-handler.h/.cpp` implement `troublemaker::ButtonHandler` — **polling-based** (called each frame). Three event types:

- `DOWN` — fires immediately on press (used sparingly)
- `SHORT` — fires on release after ≥ 20 ms (debounce floor)
- `LONG` — fires while held at 500 ms (no release needed)

`uint` is a Pico SDK typedef — safe in `.cpp` files that include Pico SDK headers; use `uint32_t`/`uint64_t` in headers.

### Button Mappings

| Screen | A-Short | A-Long | B-Short | B-Long | X-Short | X-Long | Y-Short | Y-Long |
|--------|---------|--------|---------|--------|---------|--------|---------|--------|
| Menu | Select | Reset App | — | — | Up | — | Down | — |
| Morse | — | Reset App | Reset input | Toggle clue | Next trial | Toggle input display | dit (.) | dah (-) |
| Resistor | Band left | Reset App | Band right | Band left | Value up | Toggle 4/5-band | Value down | Value up |
| Help/Top | Select | Reset App | Back to menu | — | Up | — | Down | — |
| Help/Nav | — | Reset App | Back | — | Prev page | — | Next page | — |
| Settings | Increase | Reset App | Decrease | Save + back | Prev item | — | Next item | — |
| Idle | Restart | Reset App | Resume | — | — | Toggle LED | — | — |

### Settings System

`src/settings.h` — `Settings` struct with defaults:

```cpp
struct Settings {
    uint16_t dit_dah_ms       = 250;  // dit/dah threshold (50–500, step 10)
    uint16_t idle_timeout_s   = 30;   // 0=disabled, seconds before screensaver (step 10)
    uint8_t  practice_set     = 0;    // 0=A-Z, 1=A-Z+0-9
    uint8_t  brightness       = 175;  // backlight (0–255, step 5)
    uint8_t  wrong_clue_after = 5;    // consecutive wrong answers before showing clue (1–10)
};
```

**IMPORTANT**: bump the magic constant in `settings_store.cpp` whenever this struct's layout changes.

`src/settings_store.h/.cpp` — persists to the last flash sector:

- `FlashBlock` pads to exactly `FLASH_PAGE_SIZE` (256 bytes); validated by `static_assert`
- Magic = `0xAB1E5E79`; mismatch loads defaults
- `save()` disables interrupts, erases sector, programs page, restores interrupts

### Morse Screen Details

- `TABLE` has 36 entries: A–Z (0–25) + 0–9 (26–35)
- `pick_char()` uses `settings_.practice_set` to choose pool size (26 or 36)
- dit/dah threshold from `settings_.dit_dah_ms` (not a compile-time constant)
- `wrong_streak_` tracks consecutive wrong answers; clue shown when `>= settings_.wrong_clue_after`
- On correct answer: non-blocking LED flash animation (`flash_idx_`, `flash_on_`, `flash_done_`, `flash_us_`); FLASH_DIT_MS=120, FLASH_DAH_MS=360, FLASH_GAP_MS=120
- `input_visible_` (X-Long): hides input during entry; shows correct code on CORRECT state

### Idle Screen

Rain + optional lightning screensaver (`src/screens/idle_screen.cpp`):

- 60 raindrops: each has x, y, speed (1.5–5.5 px/frame), length (3–12 px)
- Colors: BG=(0,0,0), TAIL=(0,80,100), HEAD=(180,255,255)
- Lightning: `led_enabled_` (off by default, X-Long toggles), 1–3 flash bursts, warm white (255,255,220), 2–7s between strikes

### Morse Timing Reference

- `T (ms) = 1200 / wpm`; 20 wpm → 60 ms/dit
- dah = 3× dit; letter gap = 3× dit; word gap = 7× dit

### Key Dependencies (Pimoroni libraries)

- `pico_display` / `pico_graphics` — display abstraction
- `st7789` — SPI display driver
- `rgbled` — RGB LED control
- `bitmap_fonts` / `hershey_fonts` — text rendering (`hershey_fonts.hpp` included transitively via `pico_graphics.hpp` — do **not** include it directly)
- `hardware_flash`, `hardware_sync` — flash erase/program for settings persistence
- Hardware: `hardware_spi`, `hardware_pwm`, `hardware_pio`, `hardware_dma`, `hardware_gpio`

### Large Generated File

`src/image_data.cpp` (~600 KB) contains raw image data arrays — do not edit by hand.

## Namespace

Custom code lives in the `troublemaker` namespace.
