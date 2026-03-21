//
// Created by Ken Rachynski on 2022-10-23.
//

#include "button-handler.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico_display.hpp"

using namespace troublemaker;
using namespace pimoroni;

ButtonHandler* ButtonHandler::instance_ = nullptr;

static constexpr unsigned int BUTTON_GPIOS[4] = {
    PicoDisplay::A,
    PicoDisplay::B,
    PicoDisplay::X,
    PicoDisplay::Y,
};

void ButtonHandler::init(ButtonCallback callback) {
    callback_ = std::move(callback);
    instance_ = this;

    for (int i = 0; i < 4; i++) {
        gpio_init(BUTTON_GPIOS[i]);
        gpio_set_dir(BUTTON_GPIOS[i], GPIO_IN);
        gpio_pull_up(BUTTON_GPIOS[i]);
    }

    // One callback handles all GPIO IRQs on this core
    gpio_set_irq_enabled_with_callback(BUTTON_GPIOS[0],
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);
    for (int i = 1; i < 4; i++) {
        gpio_set_irq_enabled(BUTTON_GPIOS[i],
            GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    }
}

void ButtonHandler::gpio_irq_handler(unsigned int gpio, uint32_t events) {
    if (instance_) {
        instance_->on_event(gpio, events);
    }
}

void ButtonHandler::on_event(unsigned int gpio, uint32_t events) {
    gpio_acknowledge_irq(gpio, events);

    int idx = gpio_to_index(gpio);
    if (idx < 0) return;

    // Handle both edges independently — both bits can be set in one call when bouncing
    if (events & GPIO_IRQ_EDGE_FALL) {
        // Active-low: falling edge = button pressed
        pressed_[idx] = true;
        press_start_us_[idx] = time_us_64();
    }

    if (events & GPIO_IRQ_EDGE_RISE) {
        // Rising edge = button released
        if (!pressed_[idx]) return;
        pressed_[idx] = false;

        uint32_t duration_ms = static_cast<uint32_t>(
            (time_us_64() - press_start_us_[idx]) / 1000);

        if (duration_ms < SHORT_PRESS_MS) return; // debounce

        ButtonId id = static_cast<ButtonId>(idx);
        PressType type = (duration_ms >= LONG_PRESS_MS) ? PressType::LONG : PressType::SHORT;

        if (callback_) callback_(id, type);
    }
}

int ButtonHandler::gpio_to_index(unsigned int gpio) const {
    for (int i = 0; i < 4; i++) {
        if (BUTTON_GPIOS[i] == gpio) return i;
    }
    return -1;
}
