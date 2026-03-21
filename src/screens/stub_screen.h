#pragma once

#include "../screen.h"

namespace troublemaker {

class StubScreen : public Screen {
public:
    StubScreen(pimoroni::PicoGraphics_PenRGB332& graphics, pimoroni::RGBLED& led,
               SwitchFn switch_to, const char* name);

    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    const char* name_;
};

} // namespace troublemaker
