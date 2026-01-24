#pragma once
#include "../common.h"
#include "setting_entry.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Returns true if a patch is enabled and applied, or if it's disabled and not applied; in either case it generates a verbose debugging line
 * @param setting_entry SettingEntry of patch to evaluate
 * @return boolean value indicating if either case is true
 */
bool already_done(SettingEntry setting_entry);

#define GENERATE_SETTING_ENTRY( \
    setting_id_, menu_text_, is_enabled_default_ \
) ( \
    (SettingEntry){ \
        .setting_id = (setting_id_), \
        .menu_text = (menu_text_), \
        .is_enabled_default = (is_enabled_default_), \
        .is_enabled = false, .is_applied = false \
    } \
)
