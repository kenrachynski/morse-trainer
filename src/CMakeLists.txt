# ── Sources ───────────────────────────────────────────────────────────────────

add_executable(
  ${NAME}
  morse_trainer.cpp
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

# Generate version header at build time (picks up current git SHA on every build).
# DEPENDS on .git/index so Ninja re-runs this — and recompiles about_screen.cpp —
# after every commit, in the correct order within a single cmake --build.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_generated.h
    COMMAND ${CMAKE_COMMAND}
        -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/version_generated.h
        -DSRC_DIR=${CMAKE_SOURCE_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/gen_version.cmake
    DEPENDS ${CMAKE_SOURCE_DIR}/.git/index
    VERBATIM
)
add_custom_target(gen_version DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version_generated.h)
add_dependencies(${NAME} gen_version)
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Static metadata (never changes between builds)
target_compile_definitions(${NAME} PRIVATE
    APP_VERSION="v1.0"
    APP_AUTHOR="Ken Rachynski"
    APP_NAME="Morse Trainer"
)

target_compile_options(${NAME} PRIVATE -Wall -Werror)

# Enable USB serial so picotool can reboot the device without BOOTSEL
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})