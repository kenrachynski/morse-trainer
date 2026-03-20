# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Embedded C++ firmware for a Raspberry Pi Pico running on a Pimoroni Pico Display Pack (ST7789 240×135 display + 4 buttons + RGB LED). The project is a multi-mode device: Morse Trainer, Resistor Calculator, Idle Screen, and Help/About.

## Build System

This is a CMake cross-compilation project targeting the RP2040. Both SDKs are siblings of this repo:

- **Pico SDK**: `../pico-sdk` — set `PICO_SDK_PATH` to this path
- **Pimoroni Pico libraries**: `../pimoroni-pico` — auto-detected as sibling of `PICO_SDK_PATH`

```bash
# Configure (from repo root, targeting a build directory)
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build cmake-build-debug
cmake --build cmake-build-release

# Flash (drag-and-drop the .uf2, or use picotool)
# Output artifacts are in cmake-build-debug/ or cmake-build-release/
```

The build produces `morse-trainer.uf2` (and `.elf`, `.bin`, `.hex`, `.map`) in the build directory.

Compiler flags include `-Wall -Werror` — all warnings are errors.

## Architecture

### Entry Point & Main Loop

`src/morse_trainer.cpp` contains `main()` and is currently the idle/demo screen: 100 bouncing balls, a 15-ray pinwheel, static shapes, and a cycling RGB LED. The main loop runs at ~60 fps (`sleep_ms(1000/60)`).

### Display & Graphics

Uses the Pimoroni `PicoGraphics_PenRGB332` graphics library rendered through an `ST7789` driver. All drawing happens off-screen via `graphics`, then pushed to the display with `st7789.update(&graphics)`.

### Button Handling

Four buttons: **A**, **B**, **X**, **Y** (mapped to `PicoDisplay::A/B/X/Y`).

- Short press / Long press semantics are central to the UX (short ≥ 60 ms, long ≥ 3× short = ~180 ms).
- `src/button-handler.h` / `button-handler.cpp` define `troublemaker::ButtonHandler` — currently a stub.
- The TODO in `my_pico_display.cpp` indicates buttons should move to IRQ handlers rather than polling.

### Button Mappings (from design doc)

| Screen | A-Short | A-Long | B-Short | B-Long | X-Short | X-Long | Y-Short | Y-Long |
|--------|---------|--------|---------|--------|---------|--------|---------|--------|
| Menu | Select | Reset App | — | — | Menu Up | — | Menu Down | — |
| Morse | — | Reset App | Stop/Reset | Give clue | Next Trial | — | dit | dah |
| Idle | Restart | Reset App | Balls layer ← | Balls layer → | Toggle pinwheel dir | Toggle LED | Pinwheel layer ← | Pinwheel layer → |
| Resistor | Band Left | Reset App | Band Right | Band Left (opt) | Value Up | Toggle type | Value Down | Value Up (opt) |

### Morse Timing

- `T (ms) = 1200 / wpm`
- 20 wpm → 60 ms per dit
- dah = 3× dit; letter spacing = 3× dit; word spacing = 7× dit

### Key Dependencies (Pimoroni libraries)

- `pico_display` / `pico_graphics` — display abstraction
- `st7789` — SPI display driver
- `rgbled` — RGB LED control
- `button` — button input
- `bitmap_fonts` / `hershey_fonts` — text rendering
- Hardware: `hardware_spi`, `hardware_pwm`, `hardware_pio`, `hardware_dma`

### Large Generated File

`src/image_data.cpp` (~600 KB) contains raw image data arrays — do not edit by hand.

## Namespace

Custom code lives in the `troublemaker` namespace.
