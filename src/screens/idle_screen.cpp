#include "idle_screen.h"
#include "pico/stdlib.h"
#include <cstdlib>
#include <math.h>

using namespace troublemaker;
using namespace pimoroni;

static constexpr int  BALL_COUNT = 100;
static constexpr int  RAYS       = 15;
static constexpr int  RAY_WIDTH  = 10;

IdleScreen::IdleScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn switch_to)
    : Screen(graphics, led, std::move(switch_to)) {
    init_balls();
}

void IdleScreen::on_enter() {
    init_balls();
    index_          = 0;
    direction_      = 1;
    balls_layer_    = 0;
    pinwheel_layer_ = 2;
    led_enabled_    = true;
}

void IdleScreen::init_balls() {
    balls_.clear();
    for (int i = 0; i < BALL_COUNT; i++) {
        Ball b;
        b.x   = rand() % 240;
        b.y   = rand() % 135;
        b.r   = (rand() % 10) + 3;
        b.dx  = float(rand() % 255) / 128.0f;
        b.dy  = float(rand() % 255) / 128.0f;
        b.pen = graphics_.create_pen(rand() % 255, rand() % 255, rand() % 255);
        balls_.push_back(b);
    }
}

void IdleScreen::from_hsv(float hue, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    float i = floor(hue * 6.0f);
    float f = hue * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);
    switch (int(i) % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}

void IdleScreen::draw_pinwheel(uint32_t index) {
    uint8_t red = 0, green = 0, blue = 0;
    for (int ray = 0; ray < RAYS; ray++) {
        for (int rw = 0; rw < RAY_WIDTH; rw++) {
            float rads = ((M_PI * 2.0f) / float(RAYS)) * float(ray);
            rads += float(index) / 100.0f;
            rads += float(rw)    / 100.0f;
            float cx = sin(rads) * 300.0f;
            float cy = cos(rads) * 300.0f;
            from_hsv(rads, 1.0f, 0.5f + sinf(rads / 100.0f / M_PI) * 0.5f, red, green, blue);
            graphics_.set_pen(graphics_.create_pen(red, green, blue));
            graphics_.line(Point(120, 67), Point(cx + 120, cy + 67));
        }
    }
}

void IdleScreen::update() {
    Pen BG     = graphics_.create_pen(120, 40,  60);
    Pen YELLOW = graphics_.create_pen(255, 255,  0);
    Pen TEAL   = graphics_.create_pen(  0, 255, 255);
    Pen WHITE  = graphics_.create_pen(255, 255, 255);

    graphics_.set_pen(BG);
    graphics_.clear();

    // Draw in layer order: 0=back, 1=mid (shapes always here), 2=front
    for (int pass = 0; pass < 3; pass++) {
        if (pass == balls_layer_) {
            for (auto& b : balls_) {
                b.x += b.dx;
                b.y += b.dy;
                if (b.x < 0)                    b.dx *= -1;
                if (b.x >= graphics_.bounds.w)  b.dx *= -1;
                if (b.y < 0)                    b.dy *= -1;
                if (b.y >= graphics_.bounds.h)  b.dy *= -1;
                graphics_.set_pen(b.pen);
                graphics_.circle(Point(b.x, b.y), b.r);
            }
        }
        if (pass == 1) {
            std::vector<Point> poly = {
                Point(30, 30), Point(50, 35), Point(70, 25),
                Point(80, 65), Point(50, 85), Point(30, 45),
            };
            graphics_.set_pen(YELLOW);
            graphics_.polygon(poly);
            graphics_.set_pen(TEAL);
            graphics_.triangle(Point(50, 50), Point(130, 80), Point(80, 110));
            graphics_.set_pen(WHITE);
            graphics_.line(Point(50, 50), Point(120, 80));
            graphics_.line(Point(20, 20), Point(120, 20));
            graphics_.line(Point(20, 20), Point(20, 120));
        }
        if (pass == pinwheel_layer_) {
            draw_pinwheel(index_);
        }
    }

    index_ += direction_;

    if (led_enabled_) {
        uint8_t r = 0, g = 0, b = 0;
        from_hsv(float(millis()) / 5000.0f, 1.0f, 0.5f + sinf(float(millis()) / 100.0f / M_PI) * 0.5f, r, g, b);
        led_.set_rgb(r, g, b);
    }
}

void IdleScreen::on_button(ButtonId id, PressType type) {
    if (id == ButtonId::A && type == PressType::SHORT) {
        on_enter();
    } else if (id == ButtonId::B && type == PressType::SHORT) {
        balls_layer_ = (balls_layer_ + 2) % 3; // layer ←
    } else if (id == ButtonId::B && type == PressType::LONG) {
        balls_layer_ = (balls_layer_ + 1) % 3; // layer →
    } else if (id == ButtonId::X && type == PressType::SHORT) {
        direction_ = -direction_;
    } else if (id == ButtonId::X && type == PressType::LONG) {
        led_enabled_ = !led_enabled_;
        if (!led_enabled_) led_.set_rgb(0, 0, 0);
    } else if (id == ButtonId::Y && type == PressType::SHORT) {
        pinwheel_layer_ = (pinwheel_layer_ + 2) % 3; // layer ←
    } else if (id == ButtonId::Y && type == PressType::LONG) {
        pinwheel_layer_ = (pinwheel_layer_ + 1) % 3; // layer →
    }
}
