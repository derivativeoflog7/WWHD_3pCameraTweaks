#pragma once
#include "../common.h"
#include <stdint.h>

typedef struct {
    const char *setting_id, *menu_text;
    const bool is_enabled_default;
    bool is_enabled, is_applied;
} SettingEntry;
