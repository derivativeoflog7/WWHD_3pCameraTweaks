#pragma once
#include "base_patch.h"

typedef struct {
    BasePatch base_patch_data;
    const uint32_t ORIGINAL_INSTRUCTION, REPLACEMENT_INSTRUCTION;
    uint32_t *patched_instruction_address;
} SimpleReplacementPatch;

bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region);
bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p);
