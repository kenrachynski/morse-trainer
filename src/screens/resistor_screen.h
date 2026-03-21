#pragma once

#include "../screen.h"
#include <cstdint>

namespace troublemaker {

class ResistorScreen : public Screen {
public:
    ResistorScreen(pimoroni::PicoGraphics_PenRGB332& graphics,
                   pimoroni::RGBLED& led, SwitchFn switch_to);

    void on_enter() override;
    void update() override;
    void on_button(ButtonId id, PressType type) override;

private:
    struct BandDef {
        const int* colors;
        int        count;
    };

    // Valid colour indices for each band role
    // Colour index: 0=Black 1=Brown 2=Red 3=Orange 4=Yellow
    //               5=Green 6=Blue  7=Violet 8=Grey 9=White 10=Gold 11=Silver
    static const int DIGIT1_COLORS[9];   // Brown–White (no Black as leading digit)
    static const int DIGIT_COLORS[10];   // Black–White
    static const int MULT_COLORS[12];    // all 12
    static const int TOL4_COLORS[4];     // Brown, Red, Gold, Silver
    static const int TOL5_COLORS[6];     // Brown, Red, Green, Blue, Violet, Grey

    static const BandDef BANDS_4[4];
    static const BandDef BANDS_5[5];

    static const uint8_t     COLOR_R[12];
    static const uint8_t     COLOR_G[12];
    static const uint8_t     COLOR_B[12];
    static const char* const COLOR_NAMES[12];

    static const float       MULT_VALUES[12];
    static const char* const TOL4_STR[4];
    static const char* const TOL5_STR[6];

    bool five_band_ = false;
    int  selected_  = 0;
    int  band_val_[5]{};          // index into each band's valid colour array

    int            band_count() const { return five_band_ ? 5 : 4; }
    const BandDef* band_defs()  const { return five_band_ ? BANDS_5 : BANDS_4; }
    int            color_of(int band) const;
    float          resistance()    const;
    const char*    tolerance_str() const;
    void           format_resistance(char* buf, int len) const;
    void           draw_resistor();

    void reset_defaults();
};

} // namespace troublemaker
