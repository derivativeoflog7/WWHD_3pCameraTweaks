#include "base_patch.h"
#include <coreinit/dynload.h>
#include <stdlib.h>

#define DEBUG_MESSAGE_LOOKING_FOR_RPL                       "Looking for %s"
#define DEBUG_MESSAGE_RPL_TEXT_ADDRESS                      "%s text address is at %08x"

#define DEBUG_MESSAGE_FAIL_GetNumberOfRPLs                  "OSDynLoad_GetNumberOfRPLs failed"
#define DEBUG_MESSAGE_FAIL_malloc                           "malloc failed"
#define DEBUG_MESSAGE_FAIL_GetRPLInfo                       "OSDynLoad_GetRPLInfo failed"
#define DEBUG_MESSAGE_FAIL_RPL_NOT_FOUND                    "RPL not found"

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

uint32_t get_rpl_text_address(const char name[]) {
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_LOOKING_FOR_RPL, name);
    int32_t num_rpls;

    if (!(num_rpls = OSDynLoad_GetNumberOfRPLs())) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_GetNumberOfRPLs);
        return 0xFFFFFFFF;
    }

    OSDynLoad_NotifyData *rpls;
    if ((rpls = malloc(sizeof(OSDynLoad_NotifyData) *num_rpls)) == NULL) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_malloc);
        return 0xFFFFFFFF;
    }

    if(!OSDynLoad_GetRPLInfo(0, num_rpls, rpls)) {
        free(rpls);
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_GetRPLInfo);
        return 0xFFFFFFFF;
    }

    int32_t i;
    // Init rpl_name_len to avoid gcc complaining
    int rpl_name_len = 0, patch_exec_name_len = strlen(name);
    for (i = 0; i < num_rpls; i++) {
        rpl_name_len = strlen(rpls[i].name);
        // Check if current rpl name ends with name we're looking for
        if (!strcmp(&rpls[i].name[rpl_name_len - patch_exec_name_len], name))
            break;
    }

    // If no matching rpl was found
    if (i == num_rpls) {
        free(rpls);
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_RPL_NOT_FOUND);
        return 0xFFFFFFFF;
    }

    OSDynLoad_NotifyData rpl = rpls[i];
    free(rpls);
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_RPL_TEXT_ADDRESS, &rpl.name[rpl_name_len - patch_exec_name_len], rpl.textAddr);
    return rpl.textAddr;
}

