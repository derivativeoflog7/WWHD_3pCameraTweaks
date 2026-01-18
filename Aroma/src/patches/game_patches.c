#include "../utils/logger.h"
#include "base_patch.h"
#include "game_patches.h"
#include "jump_patch.h"
#include "patch_array_element.h"
#include "patches_common_internal.h"
#include "patch_strings_internal.h"
#include "simple_replacement_patch.h"
#include <stdbool.h>
#include <stdint.h>

int apply_game_patches(GamePatches *game_patches_p, const uint64_t tid) {
    Region region;

    if (tid == game_patches_p->TID_JPN)
        region = JPN;
    else if (tid == game_patches_p->TID_USA)
        region = USA;
    else if (tid == game_patches_p->TID_EUR)
        region = EUR;
    else
        return -1;

    uint32_t rpl_text_address = get_rpl_text_address(game_patches_p->EXECUTABLE_NAME);
    if (rpl_text_address == 0xFFFFFFF)
        return -2;

    int success_count = 0;
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++)
        success_count += apply_patch_from_array(&game_patches_p->patches[i], rpl_text_address, region);

    return success_count;
}

int force_undo_game_paches(GamePatches *game_patches_p) {
    int success_count = 0;
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++)
        success_count += force_undo_patch_from_array(&game_patches_p->patches[i]);

    return success_count;
}

void unset_is_applied(GamePatches *game_patches_p) {
    for (int i = 0; i < game_patches_p->NUM_PATCHES; i++) {
        BasePatch *base_patch_data_p = get_base_patch_data_p(&game_patches_p->patches[i]);
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_RESETTING_IS_APPLIED, base_patch_data_p->SETTING_ID);
        base_patch_data_p->is_applied = false;
    }
}
