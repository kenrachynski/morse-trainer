#pragma once

#include "settings.h"

namespace troublemaker {

class SettingsStore {
public:
    static void load(Settings& s);
    static void save(const Settings& s);
};

} // namespace troublemaker
