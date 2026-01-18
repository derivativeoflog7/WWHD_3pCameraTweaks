#pragma once
#include "base_patch.h"
#include "base_patch_internal.h"
#include "simple_replacement_patch.h"
#include "jump_patch.h"

typedef enum {
    SIMPLE_REPLACEMENT_PATCH,
    JUMP_PATCH
} PatchType;

typedef struct {
    const PatchType patch_type;
    union {
        SimpleReplacementPatch simple_replacement_patch;
        JumpPatch jump_patch;
    };
} PatchArrayElement;

/**
 * Retrieve the pointer to the BasePatch of a patch inside a PatchArrayElement
 * @param element_p Pointer to the PatchArrayElement
 * @return Pointer to the BasePatch
 */
BasePatch *get_base_patch_data_p (PatchArrayElement *element_p);

#define GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, \
    original_instruction, replacement_instruction \
) ( \
    (PatchArrayElement) { \
        .patch_type = SIMPLE_REPLACEMENT_PATCH, \
        .simple_replacement_patch = (SimpleReplacementPatch) { \
            .base_patch_data = _GENERATE_BASE_PATCH_ENTRY( \
            (setting_id), (menu_text), (is_enabled_default), \
            (offset_jpn), (offset_usa), (offset_eur) \
            ), \
            .ORIGINAL_INSTRUCTION = (original_instruction), \
            .REPLACEMENT_INSTRUCTION = (replacement_instruction), \
            .patched_instruction_address = NULL \
        } \
    } \
)
