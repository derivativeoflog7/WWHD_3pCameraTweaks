#pragma once
#include "base_patch.h"
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

int apply_game_patches(GamePatches *game_patches_p, const uint64_t tid);
int force_undo_game_paches(GamePatches *game_patches_p);
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
