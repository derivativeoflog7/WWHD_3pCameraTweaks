/*  Copyright 2024 Pretendo Network contributors <pretendo.network>
 *  Copyright 2024 Ash Logan <ash@heyquark.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby
 *  granted, provided that the above copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 *  INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 *  IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *  PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Modified/adapted by derivativeoflog7, 2026
 *
 * Original code:
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.h
 */

#include "../utils/logger.h"
#include "patches_common_internal.h"
#include "patch_strings_internal.h"
#include <coreinit/dynload.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
