#include "idle_screen.h"
#include "pico/time.h"
#include <cstdlib>

using namespace troublemaker;
using namespace pimoroni;

IdleScreen::IdleScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn switch_to)
    : Screen(graphics, led, std::move(switch_to)) {
    init_drops();
}

void IdleScreen::on_enter() {
    init_drops();
    led_enabled_     = false;
    lightning_on_    = false;
    flash_remaining_ = 0;
    next_flash_us_   = time_us_64() + static_cast<uint64_t>(2000 + rand() % 3000) * 1000ULL;
    led_.set_rgb(0, 0, 0);
}

void IdleScreen::init_drops() {
    drops_.clear();
    drops_.reserve(DROP_COUNT);
    for (int i = 0; i < DROP_COUNT; i++) {
        Raindrop d;
        d.x     = static_cast<float>(rand() % 240);
        d.y     = static_cast<float>(rand() % 135);
        d.len   = 3 + rand() % 10;
        d.speed = 1.5f + static_cast<float>(rand() % 40) / 10.0f;
        drops_.push_back(d);
    }
}

void IdleScreen::update_lightning(uint64_t now) {
    if (now < next_flash_us_) return;

    if (lightning_on_) {
        led_.set_rgb(0, 0, 0);
        lightning_on_ = false;
        uint64_t gap_us = (flash_remaining_ > 0)
            ? static_cast<uint64_t>(80  + rand() % 170) * 1000ULL   // 80–250ms between flashes
            : static_cast<uint64_t>(2000 + rand() % 5000) * 1000ULL; // 2–7s between strikes
        next_flash_us_ = now + gap_us;
    } else {
        if (flash_remaining_ == 0) {
            flash_remaining_ = 1 + rand() % 3; // 1–3 flashes per strike
        }
        led_.set_rgb(255, 255, 220); // warm white flash
        lightning_on_ = true;
        flash_remaining_--;
        next_flash_us_ = now + static_cast<uint64_t>(40 + rand() % 80) * 1000ULL; // 40–120ms on
    }
}

void IdleScreen::update() {
    Pen BG   = graphics_.create_pen(  0,   0,   0);
    Pen TAIL = graphics_.create_pen(  0,  80, 100);
    Pen HEAD = graphics_.create_pen(180, 255, 255);

    graphics_.set_pen(BG);
    graphics_.clear();

    for (auto& d : drops_) {
        d.y += d.speed;
        if (d.y - d.len > 135) {
            d.y = -static_cast<float>(d.len);
            d.x = static_cast<float>(rand() % 240);
        }

        int ix = static_cast<int>(d.x);
        int iy = static_cast<int>(d.y);

        // Tail
        int tail_top = iy - d.len;
        int tail_bot = iy - 1;
        if (tail_top < 0)   tail_top = 0;
        if (tail_bot > 134) tail_bot = 134;
        if (tail_top <= tail_bot) {
            graphics_.set_pen(TAIL);
            graphics_.line(Point(ix, tail_top), Point(ix, tail_bot));
        }

        // Head pixel
        if (iy >= 0 && iy <= 134) {
            graphics_.set_pen(HEAD);
            graphics_.pixel(Point(ix, iy));
        }
    }

    if (led_enabled_) {
        update_lightning(time_us_64());
    }
}

void IdleScreen::on_button(ButtonId id, PressType type) {
    if (id == ButtonId::A && type == PressType::SHORT) {
        on_enter();
    } else if (id == ButtonId::B && type == PressType::SHORT) {
        switch_to_(ScreenId::RESUME);
    } else if (id == ButtonId::X && type == PressType::LONG) {
        led_enabled_ = !led_enabled_;
        if (!led_enabled_) led_.set_rgb(0, 0, 0);
    }
}
