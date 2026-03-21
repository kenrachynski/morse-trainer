#include "help_screen.h"
#include <cstdio>
#include <cstring>

using namespace troublemaker;
using namespace pimoroni;

// ── Static data ───────────────────────────────────────────────────────────────

const HelpScreen::NavEntry HelpScreen::NAV_MENU[] = {
    {"A-S:", "Select"},
    {"X-S:", "Up"},
    {"Y-S:", "Down"},
    {"A-L:", "Reset App"},
};

const HelpScreen::NavEntry HelpScreen::NAV_MORSE[] = {
    {"Y-S:", "dit (.)"},
    {"Y-L:", "dah (-)"},
    {"B-S:", "Reset input"},
    {"B-L:", "Toggle clue"},
    {"X-S:", "Next trial"},
    {"X-L:", "Toggle input"},
    {"A-L:", "Reset App"},
};

const HelpScreen::NavEntry HelpScreen::NAV_RESISTOR[] = {
    {"A-S:", "Band left"},
    {"B-S:", "Band right"},
    {"B-L:", "Band left"},
    {"X-S:", "Value up"},
    {"Y-S:", "Value down"},
    {"Y-L:", "Value up"},
    {"X-L:", "4/5 band"},
    {"A-L:", "Reset App"},
};

const HelpScreen::NavEntry HelpScreen::NAV_IDLE[] = {
    {"A-S:", "Restart"},
    {"B-S:", "Resume / menu"},
    {"X-L:", "LED toggle"},
    {"A-L:", "Reset App"},
};

const HelpScreen::NavPage HelpScreen::NAV_PAGES[4] = {
    {"Menu",          NAV_MENU,     4},
    {"Morse Trainer", NAV_MORSE,    7},
    {"Resistor Calc", NAV_RESISTOR, 8},
    {"Idle",          NAV_IDLE,     4},
};

const char* const HelpScreen::MORSE_CODES[26] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",  "....",
    "..",   ".---", "-.-",  ".-..", "--",   "-.",   "---",  ".--.",
    "--.-", ".-.",  "...",  "-",    "..-",  "...-", ".--",  "-..-",
    "-.--", "--.."
};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

HelpScreen::HelpScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn sw)
    : Screen(graphics, led, std::move(sw)) {}

void HelpScreen::on_enter() {
    state_    = State::TOP;
    top_sel_  = 0;
    nav_page_ = 0;
    led_.set_rgb(0, 0, 0);
}

// ── Draw helpers ──────────────────────────────────────────────────────────────

void HelpScreen::draw_top() {
    Pen WHITE  = graphics_.create_pen(255, 255, 255);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREY   = graphics_.create_pen(180, 180, 180);

    graphics_.set_pen(WHITE);
    graphics_.text("Help", Point(80, 12), 240, 3);

    const char* items[] = {"Navigation", "Morse Chart"};
    for (int i = 0; i < 2; i++) {
        int y = 62 + i * 28;
        if (i == top_sel_) {
            graphics_.set_pen(YELLOW);
            graphics_.text(">", Point(8, y), 20, 2);
            graphics_.text(items[i], Point(26, y), 240, 2);
        } else {
            graphics_.set_pen(GREY);
            graphics_.text(items[i], Point(26, y), 240, 2);
        }
    }
}

void HelpScreen::draw_nav() {
    Pen WHITE  = graphics_.create_pen(255, 255, 255);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREY   = graphics_.create_pen(200, 200, 200);

    const NavPage& page = NAV_PAGES[nav_page_];

    // Title
    graphics_.set_pen(YELLOW);
    graphics_.text(page.title, Point(5, 3), 240, 2);

    // Page indicator
    char ind[8];
    snprintf(ind, sizeof(ind), "%d/%d", nav_page_ + 1, 4);
    graphics_.set_pen(GREY);
    graphics_.text(ind, Point(200, 3), 240, 1);

    // Entries: button label left, action right
    for (int i = 0; i < page.count; i++) {
        int y = 24 + i * 13;
        graphics_.set_pen(GREY);
        graphics_.text(page.entries[i].button, Point(5,  y), 50,  1);
        graphics_.set_pen(WHITE);
        graphics_.text(page.entries[i].action, Point(50, y), 240, 1);
    }

    // Hint
    graphics_.set_pen(GREY);
    graphics_.text("X/Y: prev/next  B: back", Point(5, 122), 240, 1);
}

void HelpScreen::draw_morse() {
    Pen WHITE  = graphics_.create_pen(255, 255, 255);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREY   = graphics_.create_pen(200, 200, 200);

    // Title
    graphics_.set_pen(YELLOW);
    graphics_.text("Morse Chart", Point(5, 3), 240, 1);

    graphics_.set_pen(GREY);
    graphics_.text("B: back", Point(180, 3), 240, 1);

    // Two columns: A–M left, N–Z right
    char buf[10];
    for (int i = 0; i < 13; i++) {
        int y = 16 + i * 9;

        // Left column: A (0) – M (12)
        snprintf(buf, sizeof(buf), "%c %s", 'A' + i, MORSE_CODES[i]);
        graphics_.set_pen((i % 2 == 0) ? WHITE : GREY);
        graphics_.text(buf, Point(5, y), 115, 1);

        // Right column: N (13) – Z (25)
        snprintf(buf, sizeof(buf), "%c %s", 'N' + i, MORSE_CODES[13 + i]);
        graphics_.text(buf, Point(122, y), 240, 1);
    }
}

// ── update ────────────────────────────────────────────────────────────────────

void HelpScreen::update() {
    graphics_.set_pen(graphics_.create_pen(0, 0, 0));
    graphics_.clear();

    switch (state_) {
        case State::TOP:   draw_top();   break;
        case State::NAV:   draw_nav();   break;
        case State::MORSE: draw_morse(); break;
    }
}

// ── on_button ─────────────────────────────────────────────────────────────────

void HelpScreen::on_button(ButtonId id, PressType type) {
    if (type == PressType::DOWN) return;

    switch (state_) {
        case State::TOP:
            if (id == ButtonId::X && type == PressType::SHORT) {
                top_sel_ = (top_sel_ - 1 + 2) % 2;
            } else if (id == ButtonId::Y && type == PressType::SHORT) {
                top_sel_ = (top_sel_ + 1) % 2;
            } else if (id == ButtonId::A && type == PressType::SHORT) {
                state_ = (top_sel_ == 0) ? State::NAV : State::MORSE;
            } else if (id == ButtonId::B && type == PressType::SHORT) {
                switch_to_(ScreenId::MENU);
            }
            break;

        case State::NAV:
            if (id == ButtonId::X && type == PressType::SHORT) {
                nav_page_ = (nav_page_ - 1 + 4) % 4;
            } else if (id == ButtonId::Y && type == PressType::SHORT) {
                nav_page_ = (nav_page_ + 1) % 4;
            } else if (id == ButtonId::B && type == PressType::SHORT) {
                state_ = State::TOP;
            }
            break;

        case State::MORSE:
            if (id == ButtonId::B && type == PressType::SHORT) {
                state_ = State::TOP;
            }
            break;
    }
}
