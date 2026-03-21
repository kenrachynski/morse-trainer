#include "menu_screen.h"

using namespace troublemaker;
using namespace pimoroni;

const char* const MenuScreen::ITEMS[ITEM_COUNT] = {
    "Morse Trainer",
    "Resistor Calc",
    "Help",
    "About",
    "Settings",
};

const ScreenId MenuScreen::SCREEN_IDS[ITEM_COUNT] = {
    ScreenId::MORSE_TRAINER,
    ScreenId::RESISTOR_CALCULATOR,
    ScreenId::HELP,
    ScreenId::ABOUT,
    ScreenId::SETTINGS,
};

MenuScreen::MenuScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn switch_to)
    : Screen(graphics, led, std::move(switch_to)) {}

void MenuScreen::on_enter() {
    selected_ = 0;
    led_.set_rgb(0, 0, 0);
}

void MenuScreen::update() {
    graphics_.set_pen(graphics_.create_pen(0, 0, 0));
    graphics_.clear();

    Pen SELECTED = graphics_.create_pen(255, 255, 0);
    Pen NORMAL   = graphics_.create_pen(200, 200, 200);

    for (int i = 0; i < ITEM_COUNT; i++) {
        int y = 15 + i * 22;
        if (i == selected_) {
            graphics_.set_pen(SELECTED);
            graphics_.text(">", Point(8, y), 10);
            graphics_.text(ITEMS[i], Point(20, y), 200);
        } else {
            graphics_.set_pen(NORMAL);
            graphics_.text(ITEMS[i], Point(20, y), 200);
        }
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
