#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"
#include "button-handler.h"
#include "app.h"

using namespace pimoroni;
using namespace troublemaker;

ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);
RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);

int main() {
    st7789.set_backlight(100);

    App app(st7789, graphics, led);

    ButtonHandler buttons;
    buttons.init([&app](ButtonId id, PressType type) {
        app.on_button(id, type);
    });

    while (true) {
        buttons.poll();
        app.update();
        st7789.update(&graphics);
        sleep_ms(1000 / 60);
    }

    return 0;
}
