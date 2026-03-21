#pragma once

#include "../screen.h"
#include <vector>

namespace troublemaker {

class IdleScreen : public Screen {
public:
    IdleScreen(pimoroni::PicoGraphics_PenRGB332& graphics, pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    struct Ball {
        float x, y;
        uint8_t r;
        float dx, dy;
        uint16_t pen;
    };

    void init_balls();
    void from_hsv(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);
    void draw_pinwheel(uint32_t index);

    std::vector<Ball> balls_;
    uint32_t index_         = 0;
    int32_t  direction_     = 1;
    int      balls_layer_   = 0; // 0=back, 1=mid, 2=front
    int      pinwheel_layer_= 2;
    bool     led_enabled_   = true;
};

} // namespace troublemaker
