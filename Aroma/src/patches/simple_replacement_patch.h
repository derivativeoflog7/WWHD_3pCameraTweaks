#pragma once
#include "base_patch.h"
#include <stdint.h>

typedef struct {
    BasePatch base_patch_data;
    const uint32_t ORIGINAL_INSTRUCTION, REPLACEMENT_INSTRUCTION;
    uint32_t *patched_instruction_address;
} SimpleReplacementPatch;
