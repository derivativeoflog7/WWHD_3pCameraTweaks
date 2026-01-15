#pragma once
#include "common.h"
#include "utils/logger.h"
#include <coreinit/cache.h>
#include <coreinit/dynload.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <function_patcher/function_patching.h>
#include <kernel/kernel.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    JPN, USA, EUR, _NUM_REGIONS
} Region;

typedef enum {
    SIMPLE_REPLACEMENT_PATCH
} PatchType;


typedef struct {
    const char *SETTING_ID, *MENU_TEXT;
    const bool IS_ENABLED_DEFAULT;
    const uint32_t OFFSET_JPN, OFFSET_USA, OFFSET_EUR;
    bool is_enabled, is_applied;
} BasePatch;

typedef struct {
    BasePatch base_patch_data;
    const uint32_t ORIGINAL_INSTRUCTION, REPLACEMENT_INSTRUCTION;
    uint32_t *patched_instruction_address;
} SimpleReplacementPatch;

typedef struct {
    PatchType patch_type;
    union {
        SimpleReplacementPatch simple_replacement_patch;
    };
} PatchArrayElement;


typedef struct {
    const uint64_t TID_JPN, TID_USA, TID_EUR;
    const char *EXECUTABLE_NAME;
    const int NUM_PATCHES;
    PatchArrayElement patches[];
} GamePatches;

BasePatch *get_base_patch_data_p (PatchArrayElement *element_p);
int apply_game_patches(GamePatches *game_patches_p, const uint64_t tid);
int force_undo_game_paches(GamePatches *game_patches_p);
void unset_is_applied(GamePatches *game_patches_p);

// =====EXTERNALS DECLARED HERE=====
extern GamePatches game_patches;
