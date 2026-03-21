#include "app.h"
#include "screens/menu_screen.h"
#include "screens/idle_screen.h"
#include "screens/stub_screen.h"

using namespace troublemaker;
using namespace pimoroni;

App::App(ST7789& display, PicoGraphics_PenRGB332& graphics, RGBLED& led)
    : display_(display), graphics_(graphics), led_(led) {

    auto sw = [this](ScreenId id) { set_screen(id); };

    menu_     = new MenuScreen(graphics, led, sw);
    idle_     = new IdleScreen(graphics, led, sw);
    morse_    = new StubScreen(graphics, led, sw, "Morse Trainer");
    resistor_ = new StubScreen(graphics, led, sw, "Resistor Calc");
    help_     = new StubScreen(graphics, led, sw, "Help");
    about_    = new StubScreen(graphics, led, sw, "About");

    set_screen(ScreenId::MENU);
}

App::~App() {
    delete menu_;
    delete idle_;
    delete morse_;
    delete resistor_;
    delete help_;
    delete about_;
}

void App::on_button(ButtonId id, PressType type) {
    // A-Long resets to menu from any screen
    if (id == ButtonId::A && type == PressType::LONG) {
        set_screen(ScreenId::MENU);
        return;
    }
    if (active_) active_->on_button(id, type);
}

void App::update() {
    if (active_) active_->update();
}

void App::set_screen(ScreenId id) {
    active_ = screen_for(id);
    if (active_) active_->on_enter();
}

Screen* App::screen_for(ScreenId id) {
    switch (id) {
        case ScreenId::MENU:                return menu_;
        case ScreenId::IDLE:                return idle_;
        case ScreenId::MORSE_TRAINER:       return morse_;
        case ScreenId::RESISTOR_CALCULATOR: return resistor_;
        case ScreenId::HELP:                return help_;
        case ScreenId::ABOUT:               return about_;
        default:                            return nullptr;
    }
}
