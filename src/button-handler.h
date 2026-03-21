//
// Created by Ken Rachynski on 2022-10-23.
//

#ifndef MORSE_TRAINER_BUTTON_HANDLER_H
#define MORSE_TRAINER_BUTTON_HANDLER_H

#include <functional>
#include <cstdint>

namespace troublemaker {

enum class ButtonId { A, B, X, Y };
enum class PressType { DOWN, SHORT, LONG };

using ButtonCallback = std::function<void(ButtonId, PressType)>;

class ButtonHandler {
public:
    // Minimum hold time to register a press (debounce floor)
    static constexpr uint32_t SHORT_PRESS_MS = 20;
    // Hold time to fire a long press (fires while held, no release needed)
    static constexpr uint32_t LONG_PRESS_MS = 500;

    void init(ButtonCallback callback);
    void poll(); // call once per main loop frame

private:
    ButtonCallback callback_;
    uint64_t press_start_us_[4]{};
    bool pressed_[4]{};
    bool long_fired_[4]{};
};

} // namespace troublemaker

#endif //MORSE_TRAINER_BUTTON_HANDLER_H
