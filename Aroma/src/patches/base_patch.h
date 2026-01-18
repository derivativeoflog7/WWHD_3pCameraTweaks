#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *SETTING_ID, *MENU_TEXT;
    const bool IS_ENABLED_DEFAULT;
    const uint32_t OFFSET_JPN, OFFSET_USA, OFFSET_EUR;
    bool is_enabled, is_applied;
} BasePatch;
