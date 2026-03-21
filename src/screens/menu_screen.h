#pragma once

#include "../screen.h"

namespace troublemaker {

class MenuScreen : public Screen {
public:
    MenuScreen(pimoroni::PicoGraphics_PenRGB332& graphics, pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    static constexpr int ITEM_COUNT = 5;
    static const char* const ITEMS[ITEM_COUNT];
    static const ScreenId SCREEN_IDS[ITEM_COUNT];

    int selected_ = 0;
};

} // namespace troublemaker
