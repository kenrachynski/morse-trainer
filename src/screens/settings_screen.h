#pragma once

#include "../screen.h"
#include "../settings.h"
#include <functional>

namespace troublemaker {

class SettingsScreen : public Screen {
public:
    SettingsScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
                   pimoroni::RGBLED& led,
                   SwitchFn switch_to,
                   Settings& settings,
                   std::function<void()> on_change,
                   std::function<void()> on_save);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    static constexpr int ITEM_COUNT = 3;

    Settings&             settings_;
    std::function<void()> on_change_;
    std::function<void()> on_save_;

    int sel_ = 0;  // 0=dit_dah_ms, 1=practice_set, 2=brightness
};

} // namespace troublemaker
