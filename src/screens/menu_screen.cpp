#include "menu_screen.h"

using namespace troublemaker;
using namespace pimoroni;

const char* const MenuScreen::ITEMS[ITEM_COUNT] = {
    "Morse Trainer",
    "Resistor Calc",
    "Idle",
    "Help",
    "About",
};

const ScreenId MenuScreen::SCREEN_IDS[ITEM_COUNT] = {
    ScreenId::MORSE_TRAINER,
    ScreenId::RESISTOR_CALCULATOR,
    ScreenId::IDLE,
    ScreenId::HELP,
    ScreenId::ABOUT,
};

MenuScreen::MenuScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn switch_to)
    : Screen(graphics, led, std::move(switch_to)) {}

void MenuScreen::on_enter() {
    selected_ = 0;
}

void MenuScreen::update() {
    graphics_.set_pen(graphics_.create_pen(0, 0, 0));
    graphics_.clear();

    for (int i = 0; i < ITEM_COUNT; i++) {
        if (i == selected_) {
            graphics_.set_pen(graphics_.create_pen(255, 255, 0));
        } else {
            graphics_.set_pen(graphics_.create_pen(200, 200, 200));
        }
        graphics_.text(ITEMS[i], Point(20, 15 + i * 22), 200);
    }
}

void MenuScreen::on_button(ButtonId id, PressType type) {
    if (id == ButtonId::X && type == PressType::SHORT) {
        selected_ = (selected_ - 1 + ITEM_COUNT) % ITEM_COUNT;
    } else if (id == ButtonId::Y && type == PressType::SHORT) {
        selected_ = (selected_ + 1) % ITEM_COUNT;
    } else if (id == ButtonId::A && type == PressType::SHORT) {
        switch_to_(SCREEN_IDS[selected_]);
    }
}
