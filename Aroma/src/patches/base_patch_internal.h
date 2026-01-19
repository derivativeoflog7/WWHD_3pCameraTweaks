#pragma once
#include "../common.h"
#include "base_patch.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Returns true if a patch is enabled and applied, or if it's disabled and not applied; in either case it generates a verbose debugging line
 * @param base_patch_data BasePatch of patch to evaluate
 * @return boolean value indicating if either case is true
 */
bool already_done(BasePatch base_patch_data);

#define _GENERATE_BASE_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur \
) ( \
    (BasePatch){ \
        .SETTING_ID = (setting_id), \
        .MENU_TEXT = (menu_text), \
        .IS_ENABLED_DEFAULT = (is_enabled_default), \
        .OFFSETS[JPN] = (offset_jpn), \
        .OFFSETS[USA] = (offset_usa), \
        .OFFSETS[EUR] = (offset_eur), \
        .is_enabled = false, .is_applied = false \
    } \
)
