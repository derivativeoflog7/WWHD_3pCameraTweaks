#pragma once
#include "simple_replacement_patch.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Apply/undo a SimpleReplacementPatch, depending on its is_enabled and is_applied values, given the text address of an RPL/RPX and which region to pick the offset
 * @param patch_p Pointer to SimpleReplacementPatch, updating is_applied in case of success
 * @param rpl_text_address Address of text section of RPL
 * @param region Region to pick the offset
 * @return Boolean indicating if the patch was applied succesfully (also returns true if is_enabled and is_applied already match)
 * @see get_rpl_text_address
 */
bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region);

/**
 * Force undo a SimpleReplacementPatch (if is_applied)
 * @param patch_p Pointer to SimpleReplacementPatch, setting is_applied to false in case of success
 * @return Boolean indicating if the patch was undone succesfully (also returns true if is_applied was already false)
 */
bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p);
