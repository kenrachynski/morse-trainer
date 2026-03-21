# ── Build-time metadata ───────────────────────────────────────────────────────

execute_process(
    COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_SHA)
    set(GIT_SHA "unknown")
endif()

string(TIMESTAMP BUILD_DATE "%Y-%m-%d" UTC)

# ── Sources ───────────────────────────────────────────────────────────────────

add_executable(
  ${NAME}
  morse_trainer.cpp
  image_data.cpp
  button-handler.h button-handler.cpp
  screen.h
  settings.h
  settings_store.h settings_store.cpp
  app.h app.cpp
  screens/idle_screen.h screens/idle_screen.cpp
  screens/menu_screen.h screens/menu_screen.cpp
  screens/morse_screen.h screens/morse_screen.cpp
  screens/resistor_screen.h screens/resistor_screen.cpp
  screens/help_screen.h screens/help_screen.cpp
  screens/about_screen.h screens/about_screen.cpp
  screens/settings_screen.h screens/settings_screen.cpp
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
target_link_libraries(${NAME} pico_stdlib hardware_spi hardware_pwm hardware_pio hardware_dma hardware_gpio hardware_flash hardware_sync rgbled pico_display pico_graphics st7789)

# Inject build-time metadata
target_compile_definitions(${NAME} PRIVATE
    APP_VERSION="v1.0"
    APP_BUILD_DATE="${BUILD_DATE}"
    APP_GIT_SHA="${GIT_SHA}"
    APP_AUTHOR="Ken Rachynski"
    APP_NAME="Morse Trainer"
)

target_compile_options(${NAME} PRIVATE -Wall -Werror)

# Enable USB serial so picotool can reboot the device without BOOTSEL
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})