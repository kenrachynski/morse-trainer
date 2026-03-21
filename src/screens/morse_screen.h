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
    static constexpr uint32_t RESULT_DISPLAY_MS = 2000;

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

    char     target_       = 'A';
    char     input_[8]{};          // dots/dashes + null terminator
    int      input_len_    = 0;
    bool     clue_visible_ = false;
    State    state_        = State::WAITING;

    uint64_t y_down_us_    = 0;    // timestamp of Y press
    uint64_t last_input_us_= 0;    // timestamp of last dit/dah
    uint64_t result_us_    = 0;    // timestamp when result was shown
};

} // namespace troublemaker
