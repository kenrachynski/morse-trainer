#pragma once

#include "screen.h"
#include "settings.h"
#include "button-handler.h"
#include "drivers/st7789/st7789.hpp"

namespace troublemaker {

class MenuScreen;
class IdleScreen;
class MorseScreen;
class ResistorScreen;
class HelpScreen;
class AboutScreen;
class SettingsScreen;
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

    Settings        settings_;
    uint64_t        last_activity_us_ = 0;
    Screen*         pre_idle_         = nullptr;

    MenuScreen*     menu_;
    IdleScreen*     idle_;
    MorseScreen*    morse_;
    ResistorScreen* resistor_;
    HelpScreen*     help_;
    AboutScreen*    about_;
    SettingsScreen* settings_screen_;

    Screen* active_ = nullptr;
};

} // namespace troublemaker
