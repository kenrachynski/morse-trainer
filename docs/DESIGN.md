# Design Document

[Ken Rachynski](mailto:chief@troublemaker.dev)

## Hardware

- Raspberry Pi Pico
- Pimoroni Pico Display Pack (ST7789 240×135, 4 buttons, RGB LED)

![Pico Display Pack](images/pico-display.png)

## Menu

Items:
- Morse Trainer
- Resistor Calculator
- Help
- About
- Settings

![Menu](images/menu.png)

| Button | Action |
|--------|--------|
| A-Short | Select |
| A-Long | Reset App (→ Menu) |
| X-Short | Menu Up |
| Y-Short | Menu Down |

## Morse Trainer

### Timing

- `T (ms) = 1200 / wpm`
- 20 wpm = 60 ms / dit
- dah = 3× dit
- Letter spacing = 3× dit
- Word spacing = 7× dit
- dit/dah threshold is user-configurable in Settings (default 250 ms)

### Practice Sets

- A–Z (default)
- A–Z + 0–9

### Behaviour

- A random character is shown; player enters it in Morse using Y-Short (dit) and Y-Long (dah)
- On CORRECT: LED flashes the correct Morse code, then advances to next trial
- On WRONG: wrong_streak_ increments; after `wrong_clue_after` consecutive wrong attempts the correct Morse is shown on screen
- X-Short advances to a new character (resets streak)
- X-Long toggles input display; in blind mode the correct code is revealed on CORRECT state

![Morse Trainer](images/morse1.png)

| Button | Action |
|--------|--------|
| A-Long | Reset App |
| B-Short | Reset input / wrong streak |
| B-Long | Toggle clue |
| X-Short | Next trial |
| X-Long | Toggle input display |
| Y-Short | dit (.) |
| Y-Long | dah (-) |

## Resistor Calculator

Supports 4-band and 5-band resistors. Resistance value displayed using hershey sans font, centred on screen.

![Standard resistor](images/resistor-standard.png)
![Precision resistor](images/resistor-precision.png)

| Button | Action |
|--------|--------|
| A-Short | Band left |
| A-Long | Reset App |
| B-Short | Band right |
| B-Long | Band left |
| X-Short | Value up |
| X-Long | Toggle 4/5-band |
| Y-Short | Value down |
| Y-Long | Value up |

## Help Screen

Two sub-pages:
1. **Navigation** — button reference for each screen (4 pages, X/Y to page through, B to go back)
2. **Morse Chart** — A–Z reference in two columns

| Button | Action |
|--------|--------|
| A-Short | Select item |
| A-Long | Reset App |
| B-Short | Back to menu |
| X-Short | Up / Prev page |
| Y-Short | Down / Next page |

## About Screen

Displays:
- App name (large, yellow)
- Author name
- Version, build date, git SHA (injected at compile time via CMake)

| Button | Action |
|--------|--------|
| A-Long | Reset App |
| B-Short | Back to menu |

## Settings Screen

Persisted to flash between power cycles. Items:

| Setting | Range | Default |
|---------|-------|---------|
| Dit/Dah threshold (ms) | 50–500, step 10 | 250 |
| Practice set | A-Z / A-Z+0-9 | A-Z |
| Brightness | 0–255, step 5 | 175 |
| Show answer after N wrong | 1–10 | 5 |
| Idle timeout (s) | 0 (Off)–300, step 10 | 30 |

| Button | Action |
|--------|--------|
| A-Long | Reset App |
| A-Short | Increase value |
| B-Short | Decrease value |
| B-Long | Save and return to menu |
| X-Short | Previous item |
| Y-Short | Next item |

## Idle Screensaver

Activates automatically after `idle_timeout_s` seconds of inactivity (configurable; 0 = disabled). Not accessible from the menu — purely automatic.

Displays 60 falling raindrops with bright cyan heads and teal tails. Optional lightning: RGB LED flashes warm white (255,255,220) in 1–3 burst sequences, 2–7s apart.

| Button | Action |
|--------|--------|
| A-Short | Restart rain animation |
| A-Long | Reset App |
| B-Short | Resume previous screen (preserves state) |
| X-Long | Toggle LED lightning |

## Common Elements

### Button Press Lengths

| Type | Duration |
|------|----------|
| Debounce floor | 20 ms |
| SHORT fires | on release, after ≥ 20 ms |
| LONG fires | while held, at 500 ms |

### Reset App

A-Long from any screen returns to the Menu screen.

### Default Display

App starts at the Menu screen. After the idle timeout, the rain screensaver activates. B-Short from the screensaver resumes the previous screen without resetting its state.
