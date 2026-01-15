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
 * Modified/adapted by derivativeoflog7, 2026
 *
 * Original code:
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/patches/game_peertopeer.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/replace_mem.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.cpp
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/rpl_info.h
 */

#include "patch.h"
#include "common.h"
#include "utils/logger.h"
#include <coreinit/cache.h>
#include <coreinit/dynload.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <kernel/kernel.h>
#include <stdlib.h>
#include <string.h>

#define RPL_NAME "cking.rpx"

#define INVERT_3P_Y_AXIS_JPN_OFFSET 0x004F9DB0
#define INVERT_3P_Y_AXIS_USA_OFFSET 0x004F9DA8
#define INVERT_3P_Y_AXIS_EUR_OFFSET 0x004F9DAC

#define INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION    0xFFA00890 // fmr  f29,f1
#define INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION 0xFFA00850 // fneg f29,f1

#define CONSISTENT_3P_Y_AXIS_JPN_OFFSET 0x005102EC
#define CONSISTENT_3P_Y_AXIS_USA_OFFSET 0x005102E4
#define CONSISTENT_3P_Y_AXIS_EUR_OFFSET 0x005102E8

#define CONSISTENT_3P_Y_AXIS_ORIGINAL_INSTRUCTION 0xC19F0174 // lfs f12, +0x174(r31)


#define DEBUG_MESSAGE_RESETTING_IS_APPLIED                  "Resetting is_applied for %s"
#define DEBUG_MESSAGE_WRONG_TID                             "Not in WWHD"
#define DEBUG_MESSAGE_LOOKING_FOR_RPL                       "Looking for %s"
#define DEBUG_MESSAGE_RPL_TEXT_ADDRESS                      "%s text address is at %08x"
#define DEBUG_MESSAGE_ORIGINAL_VALUE_AT_ADDRESS             "%08x is originally %08x"
#define DEBUG_MESSAGE_PHYSICAL_ADDRESS                      "Physical address is %08x"
#define DEBUG_MESSAGE_NEW_VALUE_AT_ADDRESS                  "%08x is now %08x"
#define DEBUG_MESSAGE_DISABLED_NOT_APPLIED                  "%s is disabled and not applied"
#define DEBUG_MESSAGE_UNDOING                               "%s is diabled, undoing"
#define DEBUG_MESSAGE_ENABLED_AND_APPLIED                   "%s is enabled and already applied"
#define DEBUG_MESSAGE_APPLYING                              "%s is enabled, applying"
#define DEBUG_MESSAGE_NOT_APPLIED                           "%s is not applied"

#define APPLY                                               "Applying"
#define UNDO                                                "Undoing"
#define DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH        "%s simple replacement patch %s at address %08x"
#define DEBUG_MESSAGE_SUCCESS                               "%s %s succeeded"

#define FAIL                                                "%s %s FAILED! "
#define DEBUG_MESSAGE_FAIL_GetNumberOfRPLs                  "OSDynLoad_GetNumberOfRPLs failed"
#define DEBUG_MESSAGE_FAIL_malloc                           "malloc failed"
#define DEBUG_MESSAGE_FAIL_GetRPLInfo                       "OSDynLoad_GetRPLInfo failed"
#define DEBUG_MESSAGE_FAIL_RPL_NOT_FOUND                    "RPL not found"
#define _MISMATCH                                           "; expected %08x found %08x"
#define DEBUG_MESSAGE_FAIL_ORIGINAL_VALUE_MISMATCH FAIL     "Original value mismatch" _MISMATCH
#define DEBUG_MESSAGE_FAIL_VALUE_NOT_UPDATED FAIL           "Value did not update" _MISMATCH
#define DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS FAIL        "patched_instruction_address is NULL"

#define CONSISTENT_3P_Y_AXIS_MACHINE_CODE (uint32_t){ \
    0xC19F0174, // lfs   f12, +0x174(r31) | Load right stick Y position (original game code) \
    0xC0E10010, // lfs   f7,  +0x10(r1)   | Load zoom out target angle \
    // Zoom in target angle seems to realiably be at f26 \
    0xFC07D000, // fcmpu cr0, f7, f26     | Zoom out target angle < zoom in target angle? \
    0x40800008, // bge   skip             | If so, negate right stick Y position \
    0xFD806050, // fneg  f12, f12 \
    //skip: \
    //Next 3 instructions are from the original game code, since they've been overwritten \
    0x3CA01005, // lis   r5, 0x1005 \
    0xC0E5A7E8, // lfs   f7, -0x5818 \
    0xFC0C3800, // fcmpu cr0, f12, f7 \
    0x4E800020  // blr                    | Return to original game code \
}

uint32_t get_rpl_text_address(const char name[]);
bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final, const char *debug_prefix, const char *SETTING_ID);
bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region);
bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p);
bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region);
bool force_undo_patch_from_array(PatchArrayElement *element_p);

// =====Macros=====
#define _MAKE_BASE_PATCH_DATA( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur \
) ((BasePatch){ \
        .SETTING_ID = (setting_id), \
        .MENU_TEXT = (menu_text), \
        .IS_ENABLED_DEFAULT = (is_enabled_default), \
        .OFFSET_JPN = (offset_jpn), \
        .OFFSET_USA = (offset_usa), \
        .OFFSET_EUR = (offset_eur) \
})

#define MAKE_SIMPLE_REPLACEMENT_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, \
    original_instruction, replacement_instruction \
) ((PatchArrayElement){ \
    .patch_type = SIMPLE_REPLACEMENT_PATCH, \
    .simple_replacement_patch = (SimpleReplacementPatch){ \
        .base_patch_data = _MAKE_BASE_PATCH_DATA( \
            (setting_id), (menu_text), (is_enabled_default), \
            (offset_jpn), (offset_usa), (offset_eur) \
        ), \
        .ORIGINAL_INSTRUCTION = (original_instruction), \
        .REPLACEMENT_INSTRUCTION = (replacement_instruction) \
    } \
})

#define MAKE_GAME_PATCHES( \
    tid_jpn, tid_usa, tid_eur, executable_name, ...\
) ( \
    (GamePatches){ \
        .TID_JPN = (tid_jpn), \
        .TID_USA = (tid_usa), \
        .TID_EUR = (tid_eur), \
        .EXECUTABLE_NAME = (executable_name), \
        .patches = {__VA_ARGS__}, \
        .NUM_PATCHES = sizeof((PatchArrayElement[]){ __VA_ARGS__ }) / \
        sizeof(PatchArrayElement), \
    } \
)

// =====game_patches INITIALIZED HERE======
GamePatches game_patches = MAKE_GAME_PATCHES(
    0x0005000010143400, 0x0005000010143500, 0x0005000010143600, "cking.rpx",
    MAKE_SIMPLE_REPLACEMENT_PATCH_ENTRY(
        "invert3pYaxes",
        "Invert 3rd person camera Y axis",
        false,
        INVERT_3P_Y_AXIS_JPN_OFFSET,
        INVERT_3P_Y_AXIS_USA_OFFSET,
        INVERT_3P_Y_AXIS_EUR_OFFSET,
        INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION,
        INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION
    )
);

// =====Externally accessible functions=====
inline BasePatch *get_base_patch_data_p (PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return &element_p->simple_replacement_patch.base_patch_data;
    }

    return NULL;
}

int apply_game_patches(GamePatches *game_patches_p, const uint64_t tid) {
    Region region;

    if (tid == game_patches_p->TID_JPN)
        region = JPN;
    else if (tid == game_patches_p->TID_USA)
        region = USA;
    else if (tid == game_patches_p->TID_EUR)
        region = EUR;
    else
        return -1;

    uint32_t rpl_text_address = get_rpl_text_address(game_patches_p->EXECUTABLE_NAME);
    if (rpl_text_address == 0xFFFFFFF)
        return -2;

    int success_count = 0;
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++)
        success_count += apply_patch_from_array(&game_patches_p->patches[i], rpl_text_address, region);

    return success_count;
}

int force_undo_game_paches(GamePatches *game_patches_p) {
    int success_count = 0;
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++)
        success_count += force_undo_patch_from_array(&game_patches_p->patches[i]);

    return success_count;
}

void unset_is_applied(GamePatches *game_patches_p) {
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++) {
        BasePatch *base_patch_data_p = get_base_patch_data_p(&game_patches_p->patches[i]);
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_RESETTING_IS_APPLIED, base_patch_data_p->SETTING_ID);
        base_patch_data_p->is_applied = false;
    }
}

// =====Internal functions=====
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

bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final, const char *debug_prefix, const char *SETTING_ID) {
    // Check if initial value matches expected one
    if (*address_p != exp_initial) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_ORIGINAL_VALUE_MISMATCH, debug_prefix, SETTING_ID, exp_initial, *address_p);
        return false;
    }

    // Patch
    uint32_t physical_address = OSEffectiveToPhysical((uint32_t)address_p);
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_PHYSICAL_ADDRESS, physical_address);
    KernelCopyData(physical_address,
                   OSEffectiveToPhysical((uint32_t) &exp_final),
                   sizeof(exp_final));
    DCFlushRange(address_p, sizeof(exp_final));

    // Check if final value matches expected one
    if (*address_p != exp_final) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_VALUE_NOT_UPDATED, debug_prefix, SETTING_ID, exp_final, *address_p);
        return false;
    }

    ICInvalidateRange(address_p, sizeof(exp_final));
    return true;
}

bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region) {
    BasePatch *base_patch_data_p = &patch_p->base_patch_data;

    if (base_patch_data_p->is_enabled && base_patch_data_p->is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_ENABLED_AND_APPLIED, base_patch_data_p->SETTING_ID);
        return true;
    }

    if (!(base_patch_data_p->is_enabled || base_patch_data_p->is_applied)) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DISABLED_NOT_APPLIED, base_patch_data_p->SETTING_ID);
        return true;
    }

    uint32_t *address_p, exp_initial, exp_final;
    char *debug_prefix;

    if (base_patch_data_p->is_enabled && !base_patch_data_p->is_applied) {
        uint32_t offset;

        switch (region) {
            case JPN:
                offset = base_patch_data_p->OFFSET_JPN;
                break;
            case USA:
                offset = base_patch_data_p->OFFSET_USA;
                break;
            case EUR:
                offset = base_patch_data_p->OFFSET_EUR;
                break;
            default:
                return false;
        }

        debug_prefix = APPLY;
        exp_initial = patch_p->ORIGINAL_INSTRUCTION;
        exp_final = patch_p->REPLACEMENT_INSTRUCTION;
        address_p = (uint32_t*)(rpl_text_address + offset);
    } else {
        debug_prefix = UNDO;
        exp_initial = patch_p->REPLACEMENT_INSTRUCTION;
        exp_final = patch_p->ORIGINAL_INSTRUCTION;
        if (!patch_p->patched_instruction_address) {
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS, debug_prefix, base_patch_data_p->SETTING_ID);
            return false;
        }
        address_p = patch_p->patched_instruction_address;
    }

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH, debug_prefix, base_patch_data_p->SETTING_ID, (uint32_t)address_p);

    if (!replace_at_address(address_p, exp_initial, exp_final, debug_prefix, base_patch_data_p->SETTING_ID))
        return false;

    if (!base_patch_data_p->is_applied) {
        patch_p->patched_instruction_address = address_p;
        base_patch_data_p->is_applied = true;
    }
    else {
        patch_p->patched_instruction_address = NULL;
        base_patch_data_p->is_applied = false;
    }

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_SUCCESS, debug_prefix, base_patch_data_p->SETTING_ID);
    return true;
}

bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p) {
    BasePatch *base_patch_data_p = &patch_p->base_patch_data;

    if (!base_patch_data_p->is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_NOT_APPLIED, base_patch_data_p->SETTING_ID);
        return true;
    }

    if (!patch_p->patched_instruction_address) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS, UNDO, base_patch_data_p->SETTING_ID);
        return false;
    }

    if (!replace_at_address(patch_p->patched_instruction_address, patch_p->REPLACEMENT_INSTRUCTION, patch_p->ORIGINAL_INSTRUCTION, UNDO, base_patch_data_p->SETTING_ID))
        return false;

    base_patch_data_p->is_applied = false;

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_SUCCESS, UNDO, base_patch_data_p->SETTING_ID);
    return true;
}


bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return apply_simple_replacement_patch(&element_p->simple_replacement_patch, rpl_text_address, region);
    }

    return false; //gcc complains otherwise
}

bool force_undo_patch_from_array(PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return force_undo_simple_replacement_patch(&element_p->simple_replacement_patch);
    }
}






