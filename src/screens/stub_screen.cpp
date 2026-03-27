#include "stub_screen.h"

using namespace troublemaker;
using namespace pimoroni;

StubScreen::StubScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led,
                       SwitchFn switch_to, const char* name)
    : Screen(graphics, led, std::move(switch_to)), name_(name) {}

void StubScreen::update() {
    graphics_.set_pen(graphics_.create_pen(0, 0, 0));
    graphics_.clear();
    graphics_.set_pen(graphics_.create_pen(255, 255, 255));
    graphics_.text(name_, Point(10, 55), 220);
}

void StubScreen::on_button(ButtonId id, PressType type) {
    // B-Short returns to menu
    if (id == ButtonId::B && type == PressType::SHORT) {
        switch_to_(ScreenId::MENU);
    }
}
