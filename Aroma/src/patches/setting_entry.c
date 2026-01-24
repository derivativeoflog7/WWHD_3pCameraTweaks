#include "../utils/logger.h"
#include "setting_entry.h"
#include "setting_entry.h"
#include "patch_strings_internal.h"

bool already_done(SettingEntry setting_entry) {
    if (setting_entry.is_enabled && setting_entry.is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_ENABLED_AND_APPLIED, setting_entry.setting_id);
        return true;
    }

    if (!(setting_entry.is_enabled || setting_entry.is_applied)) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DISABLED_NOT_APPLIED, setting_entry.setting_id);
        return true;
    }
    return false;
}
