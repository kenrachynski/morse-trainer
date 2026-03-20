//
// Created by Ken Rachynski on 2022-10-23.
//

#ifndef MORSE_TRAINER_BUTTON_HANDLER_H
#define MORSE_TRAINER_BUTTON_HANDLER_H

#include <functional>
#include <cstdint>

namespace troublemaker {

enum class ButtonId { A, B, X, Y };
enum class PressType { SHORT, LONG };

using ButtonCallback = std::function<void(ButtonId, PressType)>;

class ButtonHandler {
public:
    static constexpr uint32_t SHORT_PRESS_MS = 60;
    static constexpr uint32_t LONG_PRESS_MS = 180;

    void init(ButtonCallback callback);

private:
    static void gpio_irq_handler(uint gpio, uint32_t events);
    void on_event(uint gpio, uint32_t events);
    int gpio_to_index(uint gpio) const;

    ButtonCallback callback_;
    uint64_t press_start_us_[4]{};
    bool pressed_[4]{};

    static ButtonHandler* instance_;
};

} // namespace troublemaker

#endif //MORSE_TRAINER_BUTTON_HANDLER_H
