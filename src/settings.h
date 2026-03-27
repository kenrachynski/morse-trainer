#pragma once

#include <cstdint>

namespace troublemaker {

// IMPORTANT: bump MAGIC in settings_store.cpp whenever this struct's layout changes
struct Settings {
    uint16_t dit_dah_ms       = 250;  // 50–500, step 10
    uint16_t idle_timeout_s   = 30;   // 0=disabled, else seconds before auto-idle, step 10
    uint8_t  practice_set     = 0;    // 0=A-Z, 1=A-Z+0-9
    uint8_t  brightness       = 175;  // 0–255, step 5
    uint8_t  wrong_clue_after = 5;    // show answer after this many consecutive wrong attempts (1–10)
};

} // namespace troublemaker
