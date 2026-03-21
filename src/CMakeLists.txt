add_executable(
  ${NAME}
  morse_trainer.cpp
  image_data.cpp
  button-handler.h button-handler.cpp
  screen.h
  app.h app.cpp
  screens/idle_screen.h screens/idle_screen.cpp
  screens/menu_screen.h screens/menu_screen.cpp
  screens/morse_screen.h screens/morse_screen.cpp
  screens/stub_screen.h screens/stub_screen.cpp
)

include(common/pimoroni_bus)
include(drivers/st7789/st7789)
include(drivers/rgbled/rgbled)
include(libraries/bitmap_fonts/bitmap_fonts)
include(libraries/hershey_fonts/hershey_fonts)
include(libraries/pico_display/pico_display)
include(libraries/pico_graphics/pico_graphics)


# Pull in pico libraries that we need
target_link_libraries(${NAME} pico_stdlib hardware_spi hardware_pwm hardware_pio hardware_dma hardware_gpio rgbled pico_display pico_graphics st7789)

# Enable USB serial so picotool can reboot the device without BOOTSEL
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})