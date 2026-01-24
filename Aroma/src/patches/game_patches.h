#pragma once
#include "setting_entry.h"
#include "jump_patch.h"
#include "patch_array_element.h"
#include "patch_array_element_internal.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const uint64_t TID_JPN, TID_USA, TID_EUR;
    const char *EXECUTABLE_NAME;
    const int NUM_PATCHES;
    PatchArrayElement patches[];
} GamePatches;

/**
 * Apply/undo all patches contained in a GamePatches, skipping the ones that don't need anything done
 * @param game_patches_p Pointer to GamePatches
 * @param tid Title ID of the current title
 * @return Number of patches that have been applied/undone succesfully (including ones that didn't need anything done); -1 if title ID didn't match any of the expected ones; -2 if get_rpl_text_address failed
 * @see get_rpl_text_address
 */
int apply_game_patches(GamePatches *game_patches_p, const uint64_t tid);
/**
 * Force undo all patches contained in a GamePatches, skipping the ones that aren't applied
 * @param game_patches_p Pointer to GamePatches
 * @return Number of patches that have been undone succesfully (including ones that didn't need anything done)
 */
int force_undo_game_paches(GamePatches *game_patches_p);
/**
 * Unset is_applied for all patches contained in a GamePatches (to be used when a new application is launched)
 * @param game_patches_p Pointer to GamePatches
 */
void unset_is_applied(GamePatches *game_patches_p);

// =====EXTERNALS DECLARED HERE=====
extern GamePatches game_patches;

#define GENERATE_GAME_PATCHES( \
    tid_jpn, tid_usa, tid_eur, \
    executable_name, ... \
) ( \
    (GamePatches) { \
        .TID_JPN = (tid_jpn), \
        .TID_USA = (tid_usa), \
        .TID_EUR = (tid_eur), \
        .EXECUTABLE_NAME = (executable_name), \
        .NUM_PATCHES = (int)(sizeof((PatchArrayElement[]){ __VA_ARGS__ }) / sizeof(PatchArrayElement)), \
        .patches = { __VA_ARGS__ } \
    } \
)
