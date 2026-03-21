#pragma once

#include "../screen.h"
#include "../settings.h"
#include <cstdint>

namespace troublemaker {

class MorseScreen : public Screen {
public:
    MorseScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
                pimoroni::RGBLED& led, SwitchFn switch_to,
                const Settings& settings);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

    // Time after last input before auto-decoding the sequence
    static constexpr uint32_t DECODE_TIMEOUT_MS = 1500;

    // How long to show the result before advancing/resetting
    static constexpr uint32_t RESULT_DISPLAY_MS = 1000;

    // LED flash timings for correct-answer animation
    static constexpr uint32_t FLASH_DIT_MS = 120;
    static constexpr uint32_t FLASH_DAH_MS = 360;
    static constexpr uint32_t FLASH_GAP_MS = 120;

private:
    struct MorseEntry {
        char ch;
        const char* code; // '.' = dit, '-' = dah
    };

    static const MorseEntry TABLE[];
    static constexpr int TABLE_LEN = 36; // A–Z + 0–9

    enum class State { WAITING, INPUTTING, CORRECT, WRONG };

    void        pick_char();
    const char* code_for(char ch) const;
    char        decode() const;

    const Settings& settings_;

    int      wrong_streak_ = 0;    // consecutive wrong attempts on current target
    int      flash_idx_    = 0;    // current symbol index in correct-answer LED flash
    bool     flash_on_     = false;// true while LED is on for current symbol
    bool     flash_done_   = false;// true once LED animation has completed
    uint64_t flash_us_     = 0;    // timestamp of current flash phase start

    char     target_       = 'A';
    char     input_[8]{};          // dots/dashes + null terminator
    int      input_len_    = 0;
    bool     clue_visible_  = false;
    bool     input_visible_ = true;
    State    state_        = State::WAITING;

    uint64_t y_down_us_    = 0;    // timestamp of Y press
    uint64_t last_input_us_= 0;    // timestamp of last dit/dah
    uint64_t result_us_    = 0;    // timestamp when result was shown
};

} // namespace troublemaker
