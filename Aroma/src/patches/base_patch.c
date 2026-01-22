#include "../utils/logger.h"
#include "base_patch.h"
#include "base_patch_internal.h"
#include "patch_strings_internal.h"

bool already_done(BasePatch base_patch_data) {
    if (base_patch_data.is_enabled && base_patch_data.is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_ENABLED_AND_APPLIED, base_patch_data.SETTING_ID);
        return true;
    }

    if (!(base_patch_data.is_enabled || base_patch_data.is_applied)) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DISABLED_NOT_APPLIED, base_patch_data.SETTING_ID);
        return true;
    }
    return false;
}
