#include "common.h"
#include "game_patches.h"
#include "patch_array_element.h"
#include "settings.h"
#include "simple_replacement_patch.h"
#include <coreinit/filesystem.h>
#include <coreinit/title.h>
#include <stdlib.h>
#include <wups.h>

#define DEBUG_MESSAGE_INIT_PLUGIN           "Initializing %s"
#define DEBUG_MESSAGE_STARTED_APPLICATION   "Started application with TID %08llx"
#define DEBUG_MESSAGE_DEINIT_PLUGIN         "Deinitializing %s"
#define DEBUG_MESSAGE_INIT_PATCHER_FAIL     "Failed to initliazile FunctionPatcher"

WUPS_PLUGIN_NAME(PLUGIN_NAME);
WUPS_PLUGIN_DESCRIPTION(PLUGIN_DESCRIPTION);
WUPS_PLUGIN_VERSION(PLUGIN_VERSION);
WUPS_PLUGIN_AUTHOR(PLUGIN_AUTOR);
WUPS_PLUGIN_LICENSE(PLUGIN_LICENSE);

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE(PLUGIN_CONFIG_ID);

extern void consistent3pYaxis_custom_code();

GamePatches game_patches = GENERATE_GAME_PATCHES(
    WWHD_TID_JPN, WWHD_TID_USA, WWHD_TID_EUR, WWHD_EXECUTABLE_NAME,
    GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY(
        "invert3pYaxis", "Invert Y axis", false,
        INVERT_3P_Y_AXIS_JPN_OFFSET, INVERT_3P_Y_AXIS_USA_OFFSET, INVERT_3P_Y_AXIS_EUR_OFFSET,
        INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION, INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION
    ),
    GENERATE_JUMP_PATCH_ENTRY(
        "consistent3pYaxis", "Consistent Y axis", false,
        CONSISTENT_3P_Y_AXIS_JPN_OFFSET, CONSISTENT_3P_Y_AXIS_USA_OFFSET,CONSISTENT_3P_Y_AXIS_EUR_OFFSET,
        WWHD_TID_JPN, WWHD_TID_USA, WWHD_TID_EUR,
        WWHD_EXECUTABLE_NAME, &consistent3pYaxis_custom_code
    )
);


/*void generate_patches() {
    GamePatches game_patches = (GamePatches) {
        .TID_JPN = WWHD_TID_JPN,
        .TID_USA = WWHD_TID_USA,
        .TID_EUR = WWHD_TID_EUR,
        .EXECUTABLE_NAME = WWHD_EXECUTABLE_NAME,
        .NUM_PATCHES = 2,
        .patches = (PatchArrayElement*)malloc(2 * sizeof(PatchArrayElement))
    };

    GamePatches2 gp2 = (GamePatches2) {
        .TID_JPN = WWHD_TID_JPN,
        .TID_USA = WWHD_TID_USA,
        .TID_EUR = WWHD_TID_EUR,
        .EXECUTABLE_NAME = WWHD_EXECUTABLE_NAME,
        .NUM_PATCHES = 2
    };

    PatchArrayElement invert3pYaxis = GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY(
        "invert3pYaxis", "Invert 3rd person camera Y axis", false,
        INVERT_3P_Y_AXIS_JPN_OFFSET,
        INVERT_3P_Y_AXIS_USA_OFFSET,
        INVERT_3P_Y_AXIS_EUR_OFFSET,
        INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION,
        INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION
    );
    memcpy(&game_patches.patches[0], &invert3pYaxis, sizeof(PatchArrayElement));
    memcpy(&gp2.patches[0], &invert3pYaxis, sizeof(PatchArrayElement));

    PatchArrayElement consistent3pYaxis = generate_jump_patch_array_element(
        "consistent3pYaxis", "Consistent 3rd person camera Y axis", false,
        CONSISTENT_3P_Y_AXIS_JPN_OFFSET,
        CONSISTENT_3P_Y_AXIS_USA_OFFSET,
        CONSISTENT_3P_Y_AXIS_EUR_OFFSET,
        WWHD_TID_JPN,
        WWHD_TID_USA,
        WWHD_TID_EUR,
        WWHD_EXECUTABLE_NAME,
        NULL
    );
    memcpy(&game_patches.patches[1], &consistent3pYaxis, sizeof(PatchArrayElement));
    memcpy(&gp2.patches[1], &consistent3pYaxis, sizeof(PatchArrayElement));
}*/

void on_application_start() {
    uint64_t tid = OSGetTitleID();
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_STARTED_APPLICATION, tid);
    unset_is_applied(&game_patches);
    apply_game_patches(&game_patches, tid);
}

void deinitialize_plugin() {
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DEINIT_PLUGIN, PLUGIN_NAME);
    force_undo_game_paches(&game_patches);
}


INITIALIZE_PLUGIN() {
    initLogging();
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_INIT_PLUGIN, PLUGIN_NAME);
    //generate_patches();
    initConfig();
    deinitLogging();
}

DEINITIALIZE_PLUGIN() {
    deinitialize_plugin();
    deinitLogging();
}

ON_APPLICATION_START() {
    initLogging();
    on_application_start();
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

