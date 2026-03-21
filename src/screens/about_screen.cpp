#include "about_screen.h"
#include <cstdio>

using namespace troublemaker;
using namespace pimoroni;

AboutScreen::AboutScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn sw)
    : Screen(graphics, led, std::move(sw)) {}

void AboutScreen::on_enter() {
    led_.set_rgb(0, 0, 0);
}

void AboutScreen::update() {
    Pen BG     = graphics_.create_pen(  0,   0,   0);
    Pen WHITE  = graphics_.create_pen(255, 255, 255);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREY   = graphics_.create_pen(180, 180, 180);

    graphics_.set_pen(BG);
    graphics_.clear();

    // Project name — centred, scale=3
    graphics_.set_pen(YELLOW);
    graphics_.text(APP_NAME, Point(5, 12), 230, 3);

    // Author — scale=2
    graphics_.set_pen(WHITE);
    graphics_.text(APP_AUTHOR, Point(5, 52), 230, 2);

    // Version + build date + sha — scale=1, two lines
    char ver_line[48];
    snprintf(ver_line, sizeof(ver_line), "%s (%s %s)",
             APP_VERSION, APP_BUILD_DATE, APP_GIT_SHA);
    graphics_.set_pen(GREY);
    graphics_.text(ver_line, Point(5, 90), 230, 1);

    // Hint
    graphics_.text("B: back", Point(5, 122), 230, 1);
}

void AboutScreen::on_button(ButtonId id, PressType type) {
    if (type == PressType::DOWN) return;
    if (id == ButtonId::B && type == PressType::SHORT) {
        switch_to_(ScreenId::MENU);
    }
}
