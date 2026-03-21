//
// Created by Ken Rachynski on 2022-10-23.
//

#include "button-handler.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico_display.hpp"

using namespace troublemaker;
using namespace pimoroni;

static constexpr unsigned int BUTTON_GPIOS[4] = {
    PicoDisplay::A,
    PicoDisplay::B,
    PicoDisplay::X,
    PicoDisplay::Y,
};

void ButtonHandler::init(ButtonCallback callback) {
    callback_ = std::move(callback);

    for (int i = 0; i < 4; i++) {
        gpio_init(BUTTON_GPIOS[i]);
        gpio_set_dir(BUTTON_GPIOS[i], GPIO_IN);
        gpio_pull_up(BUTTON_GPIOS[i]);
    }
}

void ButtonHandler::poll() {
    uint64_t now = time_us_64();

    for (int i = 0; i < 4; i++) {
        bool current = !gpio_get(BUTTON_GPIOS[i]); // active-low: low = pressed

        if (current && !pressed_[i]) {
            // Button just pressed
            pressed_[i]        = true;
            press_start_us_[i] = now;
            long_fired_[i]     = false;

        } else if (!current && pressed_[i]) {
            // Button just released
            pressed_[i] = false;

            if (long_fired_[i]) continue; // LONG already fired, ignore release

            uint32_t ms = static_cast<uint32_t>((now - press_start_us_[i]) / 1000);
            if (ms < SHORT_PRESS_MS) continue; // too short, discard as noise

            callback_(ButtonId(i), PressType::SHORT);

        } else if (current && pressed_[i] && !long_fired_[i]) {
            // Button held — check for long press threshold
            uint32_t ms = static_cast<uint32_t>((now - press_start_us_[i]) / 1000);
            if (ms >= LONG_PRESS_MS) {
                long_fired_[i] = true;
                callback_(ButtonId(i), PressType::LONG);
            }
        }
    }
}
