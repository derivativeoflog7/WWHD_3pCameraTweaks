#include "../utils/logger.h"
#include "setting_entry.h"
#include "setting_entry_internal.h"
#include "jump_patch.h"
#include "jump_patch_internal.h"
#include "patches_common_internal.h"
#include "patch_strings_internal.h"
#include <function_patcher/function_patching.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Logs FunctionPatcherStatus, returns true if it's FUNCTION_PATCHER_RESULT_SUCCESS and false otherwise
 * @param function_patcher_status FunctionPatcherStatus to check and log
 * @return true if FUNCTION_PATCHER_RESULT_SUCCESS, false otherwise
 */
bool jump_patch_return_and_log(FunctionPatcherStatus function_patcher_status);

bool jump_patch_return_and_log(FunctionPatcherStatus function_patcher_status) {
    switch (function_patcher_status) {
        case FUNCTION_PATCHER_RESULT_SUCCESS:
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_SUCCESS");
            return true;
        case FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND");
            break;
        case FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION");
            break;
        case FUNCTION_PATCHER_RESULT_INVALID_ARGUMENT:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_INVALID_ARGUMENT");
            break;
        case FUNCTION_PATCHER_RESULT_PATCH_NOT_FOUND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_PATCH_NOT_FOUND");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_STRUCT_VERSION:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_STRUCT_VERSION");
            break;
        case FUNCTION_PATCHER_RESULT_LIB_UNINITIALIZED:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_LIB_UNINITIALIZED");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_COMMAND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_COMMAND");
            break;
        case FUNCTION_PATCHER_RESULT_UNKNOWN_ERROR:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FUNCTION_PATCHER_RETURNED, "FUNCTION_PATCHER_RESULT_UNKNOWN_ERROR");
            break;
    }
    return false;
}

bool apply_jump_patch(JumpPatch *patch_p, Region region) {
    assert(region < NUM_REGIONS);
    SettingEntry *setting_entry_p = &patch_p->setting_entry;

    if (already_done(*setting_entry_p))
        return true;

    FunctionPatcherStatus res;
    char *debug_prefix;
    if (setting_entry_p->is_enabled) {
        // Apply
        debug_prefix = APPLY;
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DOING_JUMP_PATCH, debug_prefix, setting_entry_p->setting_id);
        res = FunctionPatcher_AddFunctionPatch(&patch_p->function_replacement_data[region], &patch_p->patched_function_handle, &setting_entry_p->is_applied);
    } else {
        // Undo
        debug_prefix = UNDO;
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DOING_JUMP_PATCH, debug_prefix, setting_entry_p->setting_id);
        if ((res = FunctionPatcher_RemoveFunctionPatch(patch_p->patched_function_handle)) == FUNCTION_PATCHER_RESULT_SUCCESS)
            setting_entry_p->is_applied = false;
    }

    bool res2 = jump_patch_return_and_log(res);
    if (res2)
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_PATCH_SUCCEEDED, debug_prefix, setting_entry_p->setting_id);
    else
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_PATCH_FAILED, debug_prefix, setting_entry_p->setting_id);
    return res2;
}

bool force_undo_jump_patch(JumpPatch *patch_p) {
    SettingEntry *setting_entry_p = &patch_p->setting_entry;

    if (!setting_entry_p->is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_NOT_APPLIED, setting_entry_p->setting_id);
        return true;
    }

    FunctionPatcherStatus res;
    if ((res = FunctionPatcher_RemoveFunctionPatch(patch_p->patched_function_handle)) == FUNCTION_PATCHER_RESULT_SUCCESS)
        setting_entry_p->is_applied = false;

    return jump_patch_return_and_log(res);
}
