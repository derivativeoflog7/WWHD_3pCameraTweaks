#pragma once
#include "../common.h"
#include <stdint.h>

typedef struct {
    const char *SETTING_ID, *MENU_TEXT;
    const bool IS_ENABLED_DEFAULT;
    bool is_enabled, is_applied;
} BasePatch;
