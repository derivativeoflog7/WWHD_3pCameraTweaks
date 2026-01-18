#pragma once
#include "simple_replacement_patch.h"
#include <stdbool.h>
#include <stdint.h>

bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region);
bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p);
