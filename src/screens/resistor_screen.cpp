#include "resistor_screen.h"
#include <cstdio>
#include <cstring>

using namespace troublemaker;
using namespace pimoroni;

// ── Static data ───────────────────────────────────────────────────────────────

const int ResistorScreen::DIGIT1_COLORS[9]  = {1,2,3,4,5,6,7,8,9};
const int ResistorScreen::DIGIT_COLORS[10]  = {0,1,2,3,4,5,6,7,8,9};
const int ResistorScreen::MULT_COLORS[12]   = {0,1,2,3,4,5,6,7,8,9,10,11};
const int ResistorScreen::TOL4_COLORS[4]    = {1,2,10,11};
const int ResistorScreen::TOL5_COLORS[6]    = {1,2,5,6,7,8};

const ResistorScreen::BandDef ResistorScreen::BANDS_4[4] = {
    {DIGIT1_COLORS, 9},
    {DIGIT_COLORS,  10},
    {MULT_COLORS,   12},
    {TOL4_COLORS,   4},
};

const ResistorScreen::BandDef ResistorScreen::BANDS_5[5] = {
    {DIGIT1_COLORS, 9},
    {DIGIT_COLORS,  10},
    {DIGIT_COLORS,  10},
    {MULT_COLORS,   12},
    {TOL5_COLORS,   6},
};

// RGB values per colour index
const uint8_t ResistorScreen::COLOR_R[12] = {  0,160,255,255,255,  0,  0,148,128,255,212,192};
const uint8_t ResistorScreen::COLOR_G[12] = {  0, 82,  0,128,255,180,  0,  0,128,255,175,192};
const uint8_t ResistorScreen::COLOR_B[12] = {  0, 45,  0,  0,  0,  0,255,211,128,255, 55,192};

const char* const ResistorScreen::COLOR_NAMES[12] = {
    "Black","Brown","Red","Orange","Yellow",
    "Green","Blue","Violet","Grey","White","Gold","Silver"
};

// Multiplier value for each colour index (0–11)
const float ResistorScreen::MULT_VALUES[12] = {
    1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f, 100000.0f,
    1000000.0f, 10000000.0f, 100000000.0f, 1000000000.0f,
    0.1f, 0.01f
};

// Tolerance strings indexed parallel to TOL4/TOL5_COLORS
const char* const ResistorScreen::TOL4_STR[4] = {"+/-1%","+/-2%","+/-5%","+/-10%"};
const char* const ResistorScreen::TOL5_STR[6] = {"+/-1%","+/-2%","+/-0.5%","+/-0.25%","+/-0.1%","+/-0.05%"};

// Band positions on screen: x offset and width
struct BandPos { int x, w; };
static const BandPos POS4[4] = {{50,15},{80,15},{110,15},{168,15}};
static const BandPos POS5[5] = {{45,12},{67,12},{89,12},{116,12},{163,12}};

// ── Lifecycle ─────────────────────────────────────────────────────────────────

ResistorScreen::ResistorScreen(PicoGraphics_PenRGB332& graphics, RGBLED& led, SwitchFn sw)
    : Screen(graphics, led, std::move(sw)) {}

void ResistorScreen::reset_defaults() {
    selected_ = 0;
    if (!five_band_) {
        // Brown, Black, Red(×100), Gold(±5%) = 1kΩ ±5%
        band_val_[0] = 0; // DIGIT1[0]  = Brown  → 1
        band_val_[1] = 0; // DIGIT[0]   = Black  → 0
        band_val_[2] = 2; // MULT[2]    = Red    → ×100
        band_val_[3] = 2; // TOL4[2]    = Gold   → ±5%
        band_val_[4] = 0;
    } else {
        // Brown, Black, Black, Red(×100), Brown(±1%) = 10kΩ ±1%
        band_val_[0] = 0; // DIGIT1[0]  = Brown → 1
        band_val_[1] = 0; // DIGIT[0]   = Black → 0
        band_val_[2] = 0; // DIGIT[0]   = Black → 0
        band_val_[3] = 2; // MULT[2]    = Red   → ×100
        band_val_[4] = 0; // TOL5[0]    = Brown → ±1%
    }
}

void ResistorScreen::on_enter() {
    five_band_ = false;
    reset_defaults();
    led_.set_rgb(0, 0, 0);
}

// ── Helpers ───────────────────────────────────────────────────────────────────

int ResistorScreen::color_of(int band) const {
    return band_defs()[band].colors[band_val_[band]];
}

float ResistorScreen::resistance() const {
    if (!five_band_) {
        float base = color_of(0) * 10.0f + color_of(1);
        return base * MULT_VALUES[color_of(2)];
    } else {
        float base = color_of(0) * 100.0f + color_of(1) * 10.0f + color_of(2);
        return base * MULT_VALUES[color_of(3)];
    }
}

const char* ResistorScreen::tolerance_str() const {
    int idx = band_val_[five_band_ ? 4 : 3];
    return five_band_ ? TOL5_STR[idx] : TOL4_STR[idx];
}

void ResistorScreen::format_resistance(char* buf, int len) const {
    float r = resistance();
    float v;
    const char* suffix;
    if      (r >= 1e9f) { v = r / 1e9f; suffix = "GR"; }
    else if (r >= 1e6f) { v = r / 1e6f; suffix = "MR"; }
    else if (r >= 1e3f) { v = r / 1e3f; suffix = "kR"; }
    else                { v = r;         suffix = "R";  }

    int whole = static_cast<int>(v);
    int frac  = static_cast<int>((v - whole) * 10.0f + 0.5f);
    if (frac >= 10) { whole++; frac = 0; }
    if (frac == 0)  snprintf(buf, len, "%d%s",     whole,       suffix);
    else            snprintf(buf, len, "%d.%d%s",  whole, frac, suffix);
}

// ── Drawing ───────────────────────────────────────────────────────────────────

void ResistorScreen::draw_resistor() {
    Pen body  = graphics_.create_pen(220, 190, 140);  // beige
    Pen wire  = graphics_.create_pen(180, 180, 180);  // grey leads
    Pen white = graphics_.create_pen(255, 255, 255);  // selection outline

    // Lead wires (centred vertically in body at y=45)
    graphics_.set_pen(wire);
    graphics_.line(Point(  0, 45), Point( 30, 45));
    graphics_.line(Point(210, 45), Point(240, 45));

    // Body (y=25 to y=65, height=40)
    graphics_.set_pen(body);
    graphics_.rectangle(Rect(30, 25, 180, 40));

    // Bands
    const BandPos* pos = five_band_ ? POS5 : POS4;
    int n = band_count();
    for (int i = 0; i < n; i++) {
        int c = color_of(i);
        graphics_.set_pen(graphics_.create_pen(COLOR_R[c], COLOR_G[c], COLOR_B[c]));
        graphics_.rectangle(Rect(pos[i].x, 25, pos[i].w, 40));
    }

    // Selection outline (drawn after all bands so it's always visible)
    {
        int x = pos[selected_].x;
        int w = pos[selected_].w;
        graphics_.set_pen(white);
        graphics_.line(Point(x - 1, 24), Point(x + w,     24)); // top
        graphics_.line(Point(x - 1, 66), Point(x + w,     66)); // bottom
        graphics_.line(Point(x - 1, 24), Point(x - 1,     66)); // left
        graphics_.line(Point(x + w, 24), Point(x + w,     66)); // right
    }
}

// ── update ────────────────────────────────────────────────────────────────────

void ResistorScreen::update() {
    Pen BG    = graphics_.create_pen( 20,  20,  20);
    Pen WHITE = graphics_.create_pen(255, 255, 255);
    Pen GREY  = graphics_.create_pen(210, 210, 210);

    graphics_.set_pen(BG);
    graphics_.clear();

    draw_resistor();

    // Colour name of selected band (in that colour, scale=2, ~12px/char)
    int c = color_of(selected_);
    const char* name = COLOR_NAMES[c];
    int nx = (240 - static_cast<int>(strlen(name)) * 12) / 2;
    if (nx < 5) nx = 5;
    // Use white for Black band name so it's readable on dark background
    uint8_t nr = (c == 0) ? 255 : COLOR_R[c];
    uint8_t ng = (c == 0) ? 255 : COLOR_G[c];
    uint8_t nb = (c == 0) ? 255 : COLOR_B[c];
    graphics_.set_pen(graphics_.create_pen(nr, ng, nb));
    graphics_.text(name, Point(nx, 74), 240, 2);

    // Resistance value — hershey font, centred across full width
    char res_buf[16];
    format_resistance(res_buf, sizeof(res_buf));
    graphics_.set_font("sans");
    graphics_.set_thickness(2);
    float rs = 1.5f;
    int rw = graphics_.measure_text(res_buf, rs);
    int rx = (240 - rw) / 2;
    if (rx < 5) rx = 5;
    graphics_.set_pen(WHITE);
    graphics_.text(res_buf, Point(rx, 108), 240, rs);
    graphics_.set_font(&font6);
    graphics_.set_thickness(1);

    // Tolerance left, band mode right, on one line below the value
    graphics_.set_pen(GREY);
    graphics_.text(tolerance_str(),                    Point(  5, 122), 120, 1);
    graphics_.text(five_band_ ? "[5-band]" : "[4-band]", Point(140, 122), 240, 1);
}

// ── on_button ─────────────────────────────────────────────────────────────────

void ResistorScreen::on_button(ButtonId id, PressType type) {
    int n = band_count();

    if ((id == ButtonId::A && type == PressType::SHORT) ||
        (id == ButtonId::B && type == PressType::LONG)) {
        selected_ = (selected_ - 1 + n) % n;                    // Band Left

    } else if (id == ButtonId::B && type == PressType::SHORT) {
        selected_ = (selected_ + 1) % n;                         // Band Right

    } else if ((id == ButtonId::X && type == PressType::SHORT) ||
               (id == ButtonId::Y && type == PressType::LONG)) {
        const BandDef& bd = band_defs()[selected_];
        band_val_[selected_] = (band_val_[selected_] + 1) % bd.count; // Value Up

    } else if (id == ButtonId::Y && type == PressType::SHORT) {
        const BandDef& bd = band_defs()[selected_];
        band_val_[selected_] = (band_val_[selected_] - 1 + bd.count) % bd.count; // Value Down

    } else if (id == ButtonId::X && type == PressType::LONG) {
        five_band_ = !five_band_;                                 // Toggle 4/5-band
        reset_defaults();
    }
}
