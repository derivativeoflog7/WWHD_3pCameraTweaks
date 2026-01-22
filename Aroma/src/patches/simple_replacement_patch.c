/*  Copyright 2023 Pretendo Network contributors <pretendo.network>
 *  Copyright 2023 Ash Logan <ash@heyquark.com>
 *  Copyright 2019 Maschell
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified/adapted by derivativeoflog7, 2026
 *
 * Original code:
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/replace_mem.cpp
 */

#include "../utils/logger.h"
#include "../common.h"
#include "base_patch.h"
#include "base_patch_internal.h"
#include "patch_strings_internal.h"
#include "simple_replacement_patch.h"
#include "simple_replacement_patch_internal.h"
#include <coreinit/cache.h>
#include <coreinit/memorymap.h>
#include <kernel/kernel.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Replace a single dword at a given address, but only if it matches the expected initial address
 * @param address_p Address of dwrod to replace
 * @param exp_initial Expected initial value
 * @param exp_final Value to put
 * @param debug_prefix Debug prefix for debugging lines
 * @param SETTING_ID setting_id of patch, for debugging lines
 * @return returns true if replacement was succesful, false if it wasn't or the address didn't contain the expected initial value
 */
bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final, const char *debug_prefix, const char *SETTING_ID);

bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final, const char *debug_prefix, const char *SETTING_ID) {
    // Check if initial value matches expected one
    if (*address_p != exp_initial) {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_ORIGINAL_VALUE_MISMATCH, exp_initial, *address_p);
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
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_VALUE_NOT_UPDATED, exp_final, *address_p);
        return false;
    }

    ICInvalidateRange(address_p, sizeof(exp_final));
    return true;
}

bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region) {
    BasePatch *base_patch_data_p = &patch_p->base_patch_data;

    if (already_done(*base_patch_data_p))
        return true;

    uint32_t *address_p, exp_initial, exp_final;
    char *debug_prefix;

    if (base_patch_data_p->is_enabled) {
        // Apply
        debug_prefix = APPLY;
        exp_initial = patch_p->ORIGINAL_INSTRUCTION;
        exp_final = patch_p->REPLACEMENT_INSTRUCTION;
        address_p = (uint32_t*)(rpl_text_address + base_patch_data_p->OFFSETS[region]);
    } else {
        // Undo
        debug_prefix = UNDO;
        exp_initial = patch_p->REPLACEMENT_INSTRUCTION;
        exp_final = patch_p->ORIGINAL_INSTRUCTION;
        if (!patch_p->patched_instruction_address) {
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS);
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
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS);
        return false;
    }

    if (!replace_at_address(patch_p->patched_instruction_address, patch_p->REPLACEMENT_INSTRUCTION, patch_p->ORIGINAL_INSTRUCTION, UNDO, base_patch_data_p->SETTING_ID))
        return false;

    base_patch_data_p->is_applied = false;

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_SUCCESS, UNDO, base_patch_data_p->SETTING_ID);
    return true;
}

