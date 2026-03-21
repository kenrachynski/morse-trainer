#pragma once

#include "screen.h"
#include "button-handler.h"
#include "drivers/st7789/st7789.hpp"

namespace troublemaker {

class MenuScreen;
class IdleScreen;
class MorseScreen;
class StubScreen;

class App {
public:
    App(pimoroni::ST7789& display, pimoroni::PicoGraphics_PenRGB332& graphics, pimoroni::RGBLED& led);
    ~App();

    void on_button(ButtonId id, PressType type);
    void update();

private:
    void set_screen(ScreenId id);
    Screen* screen_for(ScreenId id);

    pimoroni::ST7789& display_;
    pimoroni::PicoGraphics_PenRGB332& graphics_;
    pimoroni::RGBLED& led_;

    MenuScreen* menu_;
    IdleScreen* idle_;
    MorseScreen* morse_;
    StubScreen* resistor_;
    StubScreen* help_;
    StubScreen* about_;

    Screen* active_ = nullptr;
};

} // namespace troublemaker
