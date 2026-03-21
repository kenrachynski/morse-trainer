#pragma once

#include "button-handler.h"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include <functional>

namespace troublemaker {

enum class ScreenId {
    MENU,
    IDLE,
    MORSE_TRAINER,
    RESISTOR_CALCULATOR,
    HELP,
    ABOUT,
};

using SwitchFn = std::function<void(ScreenId)>;

class Screen {
public:
    Screen(pimoroni::PicoGraphics_PenRGB332& graphics, pimoroni::RGBLED& led, SwitchFn switch_to)
        : graphics_(graphics), led_(led), switch_to_(std::move(switch_to)) {}

    virtual ~Screen() = default;
    virtual void on_enter() {}
    virtual void update() = 0;
    virtual void on_button(ButtonId id, PressType type) = 0;

protected:
    pimoroni::PicoGraphics_PenRGB332& graphics_;
    pimoroni::RGBLED& led_;
    SwitchFn switch_to_;
};

} // namespace troublemaker
