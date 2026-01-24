#pragma once
#include "../common.h"
#include "setting_entry.h"
#include "patch_array_element.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Apply/undo a patch inside a PatchArrayElement, depending on its is_enabled and is_applied values, given the text address of an RPL/RPX and which region to pick the offset
 * @param patch_p Pointer to PatchArrayElement, updating is_applied of the patch in case of success
 * @param rpl_text_address Address of text section of RPL (ignored if unneeded)
 * @param region Region to pick the offset
 * @return Boolean indicating if the patch was applied succesfully (also returns true if is_enabled and is_applied already match)
 * @see get_rpl_text_address
 */
bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region);
/**
 * Force undo a patch inside a PatchArrayElement (if is_applied)
 * @param patch_p Pointer to PatchArrayElement, setting is_applied of the patch to false to false in case of success
 * @return Boolean indicating if the patch was undone succesfully (also returns true if is_applied was already false)
 */
bool force_undo_patch_from_array(PatchArrayElement *element_p);
