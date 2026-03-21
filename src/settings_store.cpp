#include "settings_store.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <cstring>

static constexpr uint32_t MAGIC           = 0xAB1E5E78;  // bump when Settings layout changes
static constexpr uint32_t SETTINGS_OFFSET = PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE;

struct FlashBlock {
    uint32_t                    magic;
    troublemaker::Settings      settings;
    uint8_t                     _pad[FLASH_PAGE_SIZE
                                     - sizeof(uint32_t)
                                     - sizeof(troublemaker::Settings)];
};

static_assert(sizeof(FlashBlock) == FLASH_PAGE_SIZE,
              "FlashBlock must fit exactly in one flash page");

using namespace troublemaker;

void SettingsStore::load(Settings& s) {
    const FlashBlock* block =
        reinterpret_cast<const FlashBlock*>(XIP_BASE + SETTINGS_OFFSET);
    if (block->magic == MAGIC) {
        s = block->settings;
    }
    // else leave s at struct defaults
}

void SettingsStore::save(const Settings& s) {
    FlashBlock block{};
    block.magic    = MAGIC;
    block.settings = s;

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(SETTINGS_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(SETTINGS_OFFSET,
                        reinterpret_cast<const uint8_t*>(&block),
                        FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}
