#pragma once

#include "../screen.h"

namespace troublemaker {

class HelpScreen : public Screen {
public:
    HelpScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
               pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    enum class State { TOP, NAV, MORSE };

    struct NavEntry {
        const char* button; // e.g. "B-S:"
        const char* action; // e.g. "Reset"
    };

    struct NavPage {
        const char*      title;
        const NavEntry*  entries;
        int              count;
    };

    static const NavEntry NAV_MENU[];
    static const NavEntry NAV_MORSE[];
    static const NavEntry NAV_RESISTOR[];
    static const NavEntry NAV_IDLE[];
    static const NavPage  NAV_PAGES[4];

    static const char* const MORSE_CODES[26]; // A–Z

    void draw_top();
    void draw_nav();
    void draw_morse();

    State state_    = State::TOP;
    int   top_sel_  = 0;  // 0=Navigation, 1=Morse Chart
    int   nav_page_ = 0;  // 0–3
};

} // namespace troublemaker
