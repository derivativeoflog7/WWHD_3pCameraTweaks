#pragma once
#include "../common.h"
#include "jump_patch.h"
#include <stdbool.h>

bool apply_jump_patch(JumpPatch *patch_p, Region region);
bool force_undo_jump_patch(JumpPatch *patch_p);
