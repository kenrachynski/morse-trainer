#pragma once

#include "../screen.h"
#include <vector>
#include <cstdint>

namespace troublemaker {

class IdleScreen : public Screen {
public:
    IdleScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
               pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    struct Raindrop {
        float x, y;
        int   len;
        float speed;
    };

    static constexpr int DROP_COUNT = 60;

    void init_drops();
    void update_lightning(uint64_t now);

    std::vector<Raindrop> drops_;

    bool     led_enabled_     = true;
    bool     lightning_on_    = false;
    int      flash_remaining_ = 0;
    uint64_t next_flash_us_   = 0;
};

} // namespace troublemaker
