#include "common.h"
#include "patches/game_patches.h"
#include "patches/jump_patch.h"
#include "patches/simple_replacement_patch.h"
#include "patches/patch_array_element.h"
#include "settings.h"
#include <coreinit/filesystem.h>
#include <coreinit/title.h>
#include <function_patcher/fpatching_defines.h>
#include <stdlib.h>
#include <wups.h>

#define DEBUG_MESSAGE_INIT_PLUGIN           "Initializing %s"
#define DEBUG_MESSAGE_STARTED_APPLICATION   "Started application with TID %08llx"
#define DEBUG_MESSAGE_DEINIT_PLUGIN         "Deinitializing %s"
#define DEBUG_MESSAGE_INIT_PATCHER_SUCESS   "Initialized FunctionPatcher"
#define DEBUG_MESSAGE_INIT_PATCHER_FAIL     "Failed to initliazile FunctionPatcher! %s"

WUPS_PLUGIN_NAME(PLUGIN_NAME);
WUPS_PLUGIN_DESCRIPTION(PLUGIN_DESCRIPTION);
WUPS_PLUGIN_VERSION(PLUGIN_VERSION);
WUPS_PLUGIN_AUTHOR(PLUGIN_AUTOR);
WUPS_PLUGIN_LICENSE(PLUGIN_LICENSE);

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE(PLUGIN_CONFIG_ID);

extern void consistent3pYaxis_custom_code();
uint32_t consistent3pYaxis_return_call = 0;

GamePatches game_patches = GENERATE_GAME_PATCHES(
    WWHD_TID_JPN, WWHD_TID_USA, WWHD_TID_EUR, WWHD_EXECUTABLE_NAME,
    GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY(
        "invert3pYaxis", "Invert 3rd person camera Y axis", false,
        INVERT_3P_Y_AXIS_JPN_OFFSET, INVERT_3P_Y_AXIS_USA_OFFSET, INVERT_3P_Y_AXIS_EUR_OFFSET,
        INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION, INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION
    ),
    GENERATE_JUMP_PATCH_ENTRY(
        "consistent3pYaxis", "Consistent 3rd person camera Y axis", false,
        CONSISTENT_3P_Y_AXIS_JPN_OFFSET, CONSISTENT_3P_Y_AXIS_USA_OFFSET, CONSISTENT_3P_Y_AXIS_EUR_OFFSET,
        WWHD_EXECUTABLE_NAME, WWHD_TID_JPN, WWHD_TID_USA, WWHD_TID_EUR,
        consistent3pYaxis_custom_code, consistent3pYaxis_return_call
    )
);

INITIALIZE_PLUGIN() {
    initLogging();
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_INIT_PLUGIN, PLUGIN_NAME);
    initConfig();
    switch (FunctionPatcher_InitLibrary()) {
        case FUNCTION_PATCHER_RESULT_SUCCESS:
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_INIT_PATCHER_SUCESS);
            break;
        case FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_INIT_PATCHER_FAIL, "FUNCTION_PATCHER_RESULT_MODULE_NOT_FOUND");
            break;
        case FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_INIT_PATCHER_FAIL, "FUNCTION_PATCHER_RESULT_MODULE_MISSING_EXPORT");
            break;
        case FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION:
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_INIT_PATCHER_FAIL, "FUNCTION_PATCHER_RESULT_UNSUPPORTED_VERSION");
            break;
        default: //Avoids gcc complaining about unhandled cases
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_INIT_PATCHER_FAIL, "Unknown error!!");
    }
    deinitLogging();
}

DEINITIALIZE_PLUGIN() {
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_DEINIT_PLUGIN, PLUGIN_NAME);
    force_undo_game_paches(&game_patches);
    FunctionPatcher_DeInitLibrary();
    deinitLogging();
}

ON_APPLICATION_START() {
    initLogging();
    uint64_t tid = OSGetTitleID();
    DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_STARTED_APPLICATION, tid);
    unset_is_applied(&game_patches);
    apply_game_patches(&game_patches, tid);
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

