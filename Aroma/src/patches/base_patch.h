#pragma once
#include "../common.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *SETTING_ID, *MENU_TEXT;
    const bool IS_ENABLED_DEFAULT;
    const uint32_t OFFSETS[NUM_REGIONS];
    bool is_enabled, is_applied;
} BasePatch;
