#include "simple_replacement_patch.h"
#include <coreinit/cache.h>
#include <coreinit/memorymap.h>
#include <kernel/kernel.h>

#define DEBUG_MESSAGE_ORIGINAL_VALUE_AT_ADDRESS             "%08x is originally %08x"
#define DEBUG_MESSAGE_PHYSICAL_ADDRESS                      "Physical address is %08x"
#define DEBUG_MESSAGE_NEW_VALUE_AT_ADDRESS                  "%08x is now %08x"
#define _MISMATCH                                           "; expected %08x found %08x"
#define DEBUG_MESSAGE_FAIL_ORIGINAL_VALUE_MISMATCH FAIL     "Original value mismatch" _MISMATCH
#define DEBUG_MESSAGE_FAIL_VALUE_NOT_UPDATED FAIL           "Value did not update" _MISMATCH
#define DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS FAIL        "patched_instruction_address is NULL"

bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final, const char *debug_prefix, const char *SETTING_ID);

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

    if (already_done(*base_patch_data_p))
        return true;

    uint32_t *address_p, exp_initial, exp_final;
    char *debug_prefix;

    if (base_patch_data_p->is_enabled) {
        // Apply
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
        // Undo
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
