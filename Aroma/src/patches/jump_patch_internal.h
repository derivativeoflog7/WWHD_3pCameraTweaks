#pragma once
#include "../common.h"
#include "jump_patch.h"
#include <stdbool.h>

/**
 * Apply/undo a JumpPatch, depending on its is_enabled and is_applied values, given the region to pick the patch
 * @param patch_p Pointer to JumpPatch, updating is_applied in case of success
 * @param region Region to pick the patch
 * @return Boolean indicating if the patch was applied succesfully (also returns true if is_enabled and is_applied already match)
 * @see get_rpl_text_address
 */
bool apply_jump_patch(JumpPatch *patch_p, Region region);

/**
 * Force undo a JumpPatch (if is_applied)
 * @param patch_p Pointer to JumpPatch, setting is_applied to false in case of success
 * @return Boolean indicating if the patch was undone succesfully (also returns true if is_applied was already false)
 */
bool force_undo_jump_patch(JumpPatch *patch_p);
