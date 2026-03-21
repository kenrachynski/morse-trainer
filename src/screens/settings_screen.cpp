#include "settings_screen.h"
#include <cstdio>

using namespace troublemaker;
using namespace pimoroni;

SettingsScreen::SettingsScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led,
                               SwitchFn sw, Settings& settings,
                               std::function<void()> on_change,
                               std::function<void()> on_save)
    : Screen(graphics, led, std::move(sw))
    , settings_(settings)
    , on_change_(std::move(on_change))
    , on_save_(std::move(on_save)) {}

void SettingsScreen::on_enter() {
    sel_ = 0;
    led_.set_rgb(0, 0, 0);
}

void SettingsScreen::update() {
    Pen BG     = graphics_.create_pen(  0,   0,   0);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREY   = graphics_.create_pen(180, 180, 180);

    graphics_.set_pen(BG);
    graphics_.clear();

    graphics_.set_pen(YELLOW);
    graphics_.text("Settings", Point(5, 5), 230, 2);

    const int ys[ITEM_COUNT] = {26, 46, 66, 86, 106};

    // Row 0: Dit/Dah threshold
    {
        char buf[24];
        snprintf(buf, sizeof(buf), "Dit/Dah: %dms", settings_.dit_dah_ms);
        graphics_.set_pen(sel_ == 0 ? YELLOW : GREY);
        if (sel_ == 0) graphics_.text(">", Point(5, ys[0]), 12, 2);
        graphics_.text(buf, Point(20, ys[0]), 230, 2);
    }

    // Row 1: Practice set
    {
        const char* val = (settings_.practice_set == 0) ? "A-Z" : "A-Z+0-9";
        char buf[24];
        snprintf(buf, sizeof(buf), "Practice: %s", val);
        graphics_.set_pen(sel_ == 1 ? YELLOW : GREY);
        if (sel_ == 1) graphics_.text(">", Point(5, ys[1]), 12, 2);
        graphics_.text(buf, Point(20, ys[1]), 230, 2);
    }

    // Row 2: Brightness
    {
        char buf[24];
        snprintf(buf, sizeof(buf), "Bright: %d", settings_.brightness);
        graphics_.set_pen(sel_ == 2 ? YELLOW : GREY);
        if (sel_ == 2) graphics_.text(">", Point(5, ys[2]), 12, 2);
        graphics_.text(buf, Point(20, ys[2]), 230, 2);
    }

    // Row 3: Wrong clue threshold
    {
        char buf[24];
        snprintf(buf, sizeof(buf), "Show ans: %d", settings_.wrong_clue_after);
        graphics_.set_pen(sel_ == 3 ? YELLOW : GREY);
        if (sel_ == 3) graphics_.text(">", Point(5, ys[3]), 12, 2);
        graphics_.text(buf, Point(20, ys[3]), 230, 2);
    }

    // Row 4: Idle timeout
    {
        char buf[24];
        if (settings_.idle_timeout_s == 0)
            snprintf(buf, sizeof(buf), "Idle: Off");
        else
            snprintf(buf, sizeof(buf), "Idle: %ds", settings_.idle_timeout_s);
        graphics_.set_pen(sel_ == 4 ? YELLOW : GREY);
        if (sel_ == 4) graphics_.text(">", Point(5, ys[4]), 12, 2);
        graphics_.text(buf, Point(20, ys[4]), 230, 2);
    }
}

void SettingsScreen::on_button(ButtonId id, PressType type) {
    if (type == PressType::DOWN) return;

    if (id == ButtonId::X && type == PressType::SHORT) {
        sel_ = (sel_ - 1 + ITEM_COUNT) % ITEM_COUNT;
        return;
    }
    if (id == ButtonId::Y && type == PressType::SHORT) {
        sel_ = (sel_ + 1) % ITEM_COUNT;
        return;
    }

    bool changed = false;

    if (id == ButtonId::A && type == PressType::SHORT) {
        switch (sel_) {
            case 0:
                if (settings_.dit_dah_ms <= 490) settings_.dit_dah_ms += 10;
                break;
            case 1:
                settings_.practice_set = (settings_.practice_set + 1) % 2;
                break;
            case 2:
                if (settings_.brightness <= 250) settings_.brightness += 5;
                else settings_.brightness = 255;
                break;
            case 3:
                if (settings_.wrong_clue_after < 10) settings_.wrong_clue_after++;
                break;
            case 4:
                settings_.idle_timeout_s = (settings_.idle_timeout_s >= 300)
                    ? 0 : settings_.idle_timeout_s + 10;
                break;
        }
        changed = true;

    } else if (id == ButtonId::B && type == PressType::SHORT) {
        switch (sel_) {
            case 0:
                if (settings_.dit_dah_ms >= 60) settings_.dit_dah_ms -= 10;
                break;
            case 1:
                settings_.practice_set = (settings_.practice_set + 1) % 2;
                break;
            case 2:
                if (settings_.brightness >= 5) settings_.brightness -= 5;
                else settings_.brightness = 0;
                break;
            case 3:
                if (settings_.wrong_clue_after > 1) settings_.wrong_clue_after--;
                break;
            case 4:
                settings_.idle_timeout_s = (settings_.idle_timeout_s == 0)
                    ? 300 : settings_.idle_timeout_s - 10;
                break;
        }
        changed = true;

    } else if (id == ButtonId::B && type == PressType::LONG) {
        on_save_();
        switch_to_(ScreenId::MENU);
        return;
    }

    if (changed) on_change_();
}
