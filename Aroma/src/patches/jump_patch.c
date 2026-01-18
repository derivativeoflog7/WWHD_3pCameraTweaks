#include "../utils/logger.h"
#include "base_patch.h"
#include "base_patch_internal.h"
#include "jump_patch.h"
#include "jump_patch_internal.h"
#include "patches_common_internal.h"
#include "patch_strings_internal.h"
#include <function_patcher/function_patching.h>
#include <stdbool.h>
#include <stdint.h>

bool jump_patch_return_and_log(FunctionPatcherStatus function_patcher_status, const char *SETTING_ID, const char *debug_prefix);

bool jump_patch_return_and_log(FunctionPatcherStatus function_patcher_status, const char *SETTING_ID, const char *debug_prefix) {
    switch (function_patcher_status) {
        case FUNCTION_PATCHER_RESULT_SUCCESS:
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_SUCCESS, debug_prefix, SETTING_ID);
            return true;
        case FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND");
            break;
        case FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION");
            break;
        case FUNCTION_PATCHER_RESULT_INVALID_ARGUMENT:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_INVALID_ARGUMENT");
            break;
        case FUNCTION_PATCHER_RESULT_PATCH_NOT_FOUND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_PATCH_NOT_FOUND");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_STRUCT_VERSION:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_STRUCT_VERSION");
            break;
        case FUNCTION_PATCHER_RESULT_LIB_UNINITIALIZED:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_LIB_UNINITIALIZED");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_COMMAND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_COMMAND");
            break;
        case FUNCTION_PATCHER_RESULT_UNKNOWN_ERROR:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR, debug_prefix, SETTING_ID, "FUNCTION_PATCHER_RESULT_UNKNOWN_ERROR");
            break;
    }
    return false;
}

bool apply_jump_patch(JumpPatch *patch_p, Region region) {
    BasePatch *base_patch_data_p = &patch_p->base_patch_data;

    if (already_done(*base_patch_data_p))
        return true;

    function_replacement_data_t *target_function_replacement_data;

    switch (region) {
        case JPN:
            target_function_replacement_data = &patch_p->function_replacement_data_JPN;
            break;
        case USA:
            target_function_replacement_data = &patch_p->function_replacement_data_USA;
            break;
        case EUR:
            target_function_replacement_data = &patch_p->function_replacement_data_EUR;
            break;
        default:
            return false;
    }

    FunctionPatcherStatus res;
    char *debug_prefix;
    if (base_patch_data_p->is_enabled) {
        // Apply
        debug_prefix = APPLY;
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DOING_JUMP_PATCH, debug_prefix, base_patch_data_p->SETTING_ID);
        res = FunctionPatcher_AddFunctionPatch(target_function_replacement_data, &patch_p->patched_function_handle, &base_patch_data_p->is_applied);
    } else {
        // Undo
        debug_prefix = UNDO;
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DOING_JUMP_PATCH, debug_prefix, base_patch_data_p->SETTING_ID);
        if ((res = FunctionPatcher_RemoveFunctionPatch(patch_p->patched_function_handle)) == FUNCTION_PATCHER_RESULT_SUCCESS)
            base_patch_data_p->is_applied = false;
    }

    return jump_patch_return_and_log(res, base_patch_data_p->SETTING_ID, debug_prefix);
}

bool force_undo_jump_patch(JumpPatch *patch_p) {
    BasePatch *base_patch_data_p = &patch_p->base_patch_data;

    if (!base_patch_data_p->is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_NOT_APPLIED, base_patch_data_p->SETTING_ID);
        return true;
    }

    FunctionPatcherStatus res;
    if ((res = FunctionPatcher_RemoveFunctionPatch(patch_p->patched_function_handle)) == FUNCTION_PATCHER_RESULT_SUCCESS)
        base_patch_data_p->is_applied = false;

    return jump_patch_return_and_log(res, base_patch_data_p->SETTING_ID, UNDO);
}
