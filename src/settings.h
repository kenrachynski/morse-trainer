#pragma once

#include <cstdint>

namespace troublemaker {

struct Settings {
    uint16_t dit_dah_ms   = 250;  // 50–500, step 10
    uint8_t  practice_set = 0;    // 0=A-Z, 1=A-Z+0-9
    uint8_t  brightness   = 175;  // 0–255, step 5
};

} // namespace troublemaker
