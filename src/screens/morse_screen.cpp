#include "morse_screen.h"
#include "pico/time.h"
#include <cstdlib>
#include <cstring>

using namespace troublemaker;
using namespace pimoroni;

const MorseScreen::MorseEntry MorseScreen::TABLE[TABLE_LEN] = {
    {'A', ".-"},   {'B', "-..."},  {'C', "-.-."},
    {'D', "-.."},  {'E', "."},     {'F', "..-."},
    {'G', "--."},  {'H', "...."},  {'I', ".."},
    {'J', ".---"}, {'K', "-.-"},   {'L', ".-.."},
    {'M', "--"},   {'N', "-."},    {'O', "---"},
    {'P', ".--."}, {'Q', "--.-"},  {'R', ".-."},
    {'S', "..."},  {'T', "-"},     {'U', "..-"},
    {'V', "...-"}, {'W', ".--"},   {'X', "-..-"},
    {'Y', "-.--"}, {'Z', "--.."},
};

MorseScreen::MorseScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn switch_to)
    : Screen(graphics, led, std::move(switch_to)) {}

void MorseScreen::on_enter() {
    srand(static_cast<unsigned int>(time_us_64()));
    pick_char();
    input_len_     = 0;
    input_[0]      = '\0';
    clue_visible_  = false;
    state_         = State::WAITING;
    y_down_us_     = 0;
    last_input_us_ = 0;
    result_us_     = 0;
    led_.set_rgb(0, 0, 0);
}

void MorseScreen::pick_char() {
    char next;
    do { next = 'A' + (rand() % 26); } while (next == target_);
    target_ = next;
}

const char* MorseScreen::code_for(char ch) const {
    for (int i = 0; i < TABLE_LEN; i++) {
        if (TABLE[i].ch == ch) return TABLE[i].code;
    }
    return "";
}

char MorseScreen::decode() const {
    for (int i = 0; i < TABLE_LEN; i++) {
        if (strcmp(TABLE[i].code, input_) == 0) return TABLE[i].ch;
    }
    return '?';
}

void MorseScreen::update() {
    uint64_t now = time_us_64();

    // Auto-decode after silence
    if (state_ == State::INPUTTING && input_len_ > 0) {
        uint32_t ms = static_cast<uint32_t>((now - last_input_us_) / 1000);
        if (ms >= DECODE_TIMEOUT_MS) {
            if (decode() == target_) {
                state_     = State::CORRECT;
                result_us_ = now;
                led_.set_rgb(0, 200, 0);
            } else {
                state_     = State::WRONG;
                result_us_ = now;
                led_.set_rgb(200, 0, 0);
            }
        }
    }

    // Auto-advance after correct
    if (state_ == State::CORRECT) {
        uint32_t ms = static_cast<uint32_t>((now - result_us_) / 1000);
        if (ms >= RESULT_DISPLAY_MS) {
            pick_char();
            input_len_    = 0;
            input_[0]     = '\0';
            clue_visible_ = false;
            state_        = State::WAITING;
            led_.set_rgb(0, 0, 0);
        }
    }

    // Auto-reset after wrong
    if (state_ == State::WRONG) {
        uint32_t ms = static_cast<uint32_t>((now - result_us_) / 1000);
        if (ms >= RESULT_DISPLAY_MS) {
            input_len_ = 0;
            input_[0]  = '\0';
            state_     = State::WAITING;
            led_.set_rgb(0, 0, 0);
        }
    }

    // ── Draw ─────────────────────────────────────────────────────────────
    Pen BG     = graphics_.create_pen(  0,   0,  30);
    Pen WHITE  = graphics_.create_pen(255, 255, 255);
    Pen YELLOW = graphics_.create_pen(255, 255,   0);
    Pen GREEN  = graphics_.create_pen(  0, 220,   0);
    Pen RED    = graphics_.create_pen(255,  60,  60);
    Pen GREY   = graphics_.create_pen(150, 150, 150);

    graphics_.set_pen(BG);
    graphics_.clear();

    // Target character — centred, large (scale 4 ≈ 32px wide per char)
    char target_str[2] = { target_, '\0' };
    graphics_.set_pen(WHITE);
    graphics_.text(target_str, Point(104, 15), 240, 4);

    // Input sequence — centred at mid-screen
    if (input_len_ > 0) {
        // scale=2: ~12px per symbol; rough centring
        int x = (240 - input_len_ * 12) / 2;
        if (x < 5) x = 5;
        graphics_.set_pen(YELLOW);
        graphics_.text(input_, Point(x, 65), 240, 2);
    }

    // Clue (toggle with B-Long) — shown below input
    if (clue_visible_ && state_ != State::CORRECT && state_ != State::WRONG) {
        const char* code = code_for(target_);
        int x = (240 - static_cast<int>(strlen(code)) * 12) / 2;
        if (x < 5) x = 5;
        graphics_.set_pen(GREY);
        graphics_.text(code, Point(x, 95), 240, 2);
    }

    // Result feedback
    if (state_ == State::CORRECT) {
        graphics_.set_pen(GREEN);
        graphics_.text("correct!", Point(60, 95), 240, 2);
    } else if (state_ == State::WRONG) {
        graphics_.set_pen(RED);
        graphics_.text("wrong", Point(87, 90), 240, 2);
        const char* code = code_for(target_);
        int x = (240 - static_cast<int>(strlen(code)) * 12) / 2;
        if (x < 5) x = 5;
        graphics_.set_pen(GREEN);
        graphics_.text(code, Point(x, 112), 240, 2);
    }
}

void MorseScreen::on_button(ButtonId id, PressType type) {
    uint64_t now = time_us_64();

    // Record Y press time for dit/dah measurement
    if (id == ButtonId::Y && type == PressType::DOWN) {
        y_down_us_ = now;
        return;
    }

    // Y release or held long enough: classify dit vs dah
    if (id == ButtonId::Y && (type == PressType::SHORT || type == PressType::LONG)) {
        if (state_ == State::CORRECT) return;
        // WRONG: dismiss feedback and start fresh attempt
        if (state_ == State::WRONG) {
            input_len_ = 0;
            input_[0]  = '\0';
            state_     = State::WAITING;
            led_.set_rgb(0, 0, 0);
        }
        if (input_len_ >= 7) return;

        char symbol;
        if (type == PressType::LONG) {
            symbol = '-'; // held past 500ms, always dah
        } else {
            uint32_t ms = static_cast<uint32_t>((now - y_down_us_) / 1000);
            symbol = (ms >= DIT_DAH_THRESHOLD_MS) ? '-' : '.';
        }

        input_[input_len_++] = symbol;
        input_[input_len_]   = '\0';
        last_input_us_       = now;
        state_               = State::INPUTTING;
        return;
    }

    if (id == ButtonId::B && type == PressType::SHORT) {
        // Stop/reset: clear current input
        input_len_ = 0;
        input_[0]  = '\0';
        state_     = State::WAITING;
        led_.set_rgb(0, 0, 0);
    } else if (id == ButtonId::B && type == PressType::LONG) {
        clue_visible_ = !clue_visible_;
    } else if (id == ButtonId::X && type == PressType::SHORT) {
        // Next trial
        pick_char();
        input_len_    = 0;
        input_[0]     = '\0';
        clue_visible_ = false;
        state_        = State::WAITING;
        led_.set_rgb(0, 0, 0);
    }
}
