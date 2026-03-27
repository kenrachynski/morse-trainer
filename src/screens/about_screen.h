#pragma once

#include "../screen.h"

namespace troublemaker {

class AboutScreen : public Screen {
public:
    AboutScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
                pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;
};

} // namespace troublemaker
