#include "../common.h"
#include <coreinit/cache.h>
#include <coreinit/memorymap.h>
#include <kernel/kernel.h>
#include <stdint.h>
#include "patch_strings_internal.h"
#include "replacement_data_internal.h"

/**
 * Replace a single dword at a given address, but only if it matches the expected initial address
 * @param address_p Address of dwrod to replace
 * @param exp_initial Expected initial value
 * @param exp_final Value to put
 * @return returns true if replacement was succesful, false if it wasn't or the address didn't contain the expected initial value
 */
bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final);

bool replace_at_address(uint32_t *address_p, uint32_t exp_initial, uint32_t exp_final) {
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

bool apply_replacement(ReplacementData *replacement_data_p, uint32_t rpl_text_address, Region region) {
    uint32_t *address = (uint32_t*)(rpl_text_address+replacement_data_p->offsets[region]);
    bool res = replace_at_address(address, replacement_data_p->original_value, replacement_data_p->replacement_value);


    if (res)
        replacement_data_p->replaced_data_pointer = address;
    return res;
}

bool undo_replacement(ReplacementData *replacement_data_p) {
    bool res = replace_at_address(replacement_data_p->replaced_data_pointer, replacement_data_p->replacement_value, replacement_data_p->original_value);

    if (res)
        replacement_data_p->replaced_data_pointer = NULL;
    return res;
}
