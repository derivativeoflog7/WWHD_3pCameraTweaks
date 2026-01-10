/*  Copyright 2024 Pretendo Network contributors <pretendo.network>
 *  Copyright 2024 Ash Logan <ash@heyquark.com>
 *  Copyright 2019 Maschell
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
 * Modified by derivativeoflog7, 10 Jan 2026
 *
 * Original code:
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/patches/game_peertopeer.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/replace_mem.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.h
 */

#include "patch.h"
#include "settings.h"
#include "utils/logger.h"
#include <coreinit/dynload.h>
#include <coreinit/cache.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <kernel/kernel.h>
#include <optional>
#include <cstdint>
#include <string_view>
#include <vector>
#include <algorithm>
#include <string_view>

#define TID_JPN 0x00050000'10143400
#define TID_USA 0x00050000'10143500
#define TID_EUR 0x00050000'10143600

#define JPN_OFFSET 0x004F9DB0
#define USA_OFFSET 0x004F9DA8
#define EUR_OFFSET 0x004F9DAC

#define ORIGINAL_INST 0xFFA00890 // fmr  f29,f1
#define PATCHED_INST  0xFFA00850 // fneg f29,f1

using namespace std::string_view_literals;

PatchResult replace_instruction(uint32_t *address, uint32_t original_value, uint32_t new_value) {
    DEBUG_FUNCTION_LINE_VERBOSE("%08x is originally %08x", address, *address);
    if (*address != original_value){
        DEBUG_FUNCTION_LINE_ERR("Original value mismatch");
        return FAIL_ORIGINAL_VALUE_MISMATCH;
    }

    KernelCopyData(
        OSEffectiveToPhysical((uint32_t) address),
                   OSEffectiveToPhysical((uint32_t) &new_value),
                   sizeof(new_value)
    );
    DCFlushRange(address, sizeof(new_value));
    DEBUG_FUNCTION_LINE_VERBOSE("%08x is now %08x", address, *address);
    if (*address != new_value) {
        DEBUG_FUNCTION_LINE_ERR("Value not updated");
        return FAIL_VALUE_NOT_UPDATED;
    }

    ICInvalidateRange(address, sizeof(new_value));
    return SUCCESS;
}

std::optional<OSDynLoad_NotifyData> search_for_rpl(std::string_view name) {
    int num_rpls = OSDynLoad_GetNumberOfRPLs();
    if (!num_rpls)
        return std::nullopt;

    // allocates but we free it at return
    std::vector<OSDynLoad_NotifyData> rpls(num_rpls);
    if (!OSDynLoad_GetRPLInfo(0, num_rpls, rpls.data()))
        return std::nullopt;

    auto rpl = std::find_if(rpls.begin(), rpls.end(), [=](const OSDynLoad_NotifyData &rpl) {
        return std::string_view(rpl.name).ends_with(name);
    });
    if (rpl == rpls.end())
        return std::nullopt;

    return *rpl;
}

PatchResult replace(uint32_t offset, uint32_t original_value, uint32_t new_value) {
    uint32_t *address;
    std::optional<OSDynLoad_NotifyData> rpl = search_for_rpl("cking.rpx"sv);
    if (!rpl) {
        DEBUG_FUNCTION_LINE_ERR("rpl not found");
        return FAIL_NO_RPL;
    }
    //TODO add version check;

    OSDynLoad_NotifyData rpl2 = rpl.value();
    DEBUG_FUNCTION_LINE_VERBOSE("rpl text is at address %08x", rpl2.textAddr);
    address = (uint32_t*)(rpl2.textAddr + offset);
    return replace_instruction(address, original_value, new_value);
}

PatchResult patch(bool undo) {
    uint64_t tid = OSGetTitleID();
    uint32_t original = ORIGINAL_INST, neww = PATCHED_INST, offset;

    switch (tid) {
        case TID_JPN:
            offset = JPN_OFFSET;
            break;
        case TID_USA:
            offset = USA_OFFSET;
            break;
        case TID_EUR:
            offset = EUR_OFFSET;
            break;
        default:
            return NOT_WWHD;
    }

    if (undo) {
        original = PATCHED_INST;
        neww = ORIGINAL_INST;
    }


    DEBUG_FUNCTION_LINE_VERBOSE("Patching with undo = %s, offset = %08x, original = %08x, new = %08x",
                                undo ? "true", "false",
                                original,
                                neww);
    return replace(offset, original, neww);
}

