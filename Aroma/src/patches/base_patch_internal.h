#pragma once
#include "base_patch.h"
#include <stdbool.h>
#include <stdint.h>

bool already_done(BasePatch base_patch_data);

#define _GENERATE_BASE_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur \
) ( \
    (BasePatch){ \
        .SETTING_ID = (setting_id), \
        .MENU_TEXT = (menu_text), \
        .IS_ENABLED_DEFAULT = (is_enabled_default), \
        .OFFSET_JPN = (offset_jpn), \
        .OFFSET_USA = (offset_usa), \
        .OFFSET_EUR = (offset_eur), \
        .is_enabled = false, .is_applied = false \
    } \
)
