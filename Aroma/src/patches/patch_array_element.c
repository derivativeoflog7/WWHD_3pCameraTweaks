#include "../utils/logger.h"
#include "base_patch.h"
#include "jump_patch_internal.h"
#include "patch_array_element.h"
#include "patch_array_element_internal.h"
#include "simple_replacement_patch_internal.h"
#include <stdbool.h>
#include <stdint.h>

inline BasePatch *get_base_patch_data_p (PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return &element_p->simple_replacement_patch.base_patch_data;
        case JUMP_PATCH:
            return &element_p->jump_patch.base_patch_data;
    }

    return NULL; //gcc complains otherwise
}

bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return apply_simple_replacement_patch(&element_p->simple_replacement_patch, rpl_text_address, region);
        case JUMP_PATCH:
            return apply_jump_patch(&element_p->jump_patch, region);
    }

    return false; //gcc complains otherwise
}

bool force_undo_patch_from_array(PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return force_undo_simple_replacement_patch(&element_p->simple_replacement_patch);
        case JUMP_PATCH:
            return force_undo_jump_patch(&element_p->jump_patch);
    }

    return false; //gcc complains otherwise
}
