#pragma once
#include "../common.h"
#include "base_patch.h"
#include "patch_array_element.h"
#include <stdbool.h>
#include <stdint.h>

bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region);
bool force_undo_patch_from_array(PatchArrayElement *element_p);
