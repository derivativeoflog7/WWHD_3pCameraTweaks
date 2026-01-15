#include "common.h"
#include "patch.h"
#include "settings.h"
#include "utils/logger.h"
#include <coreinit/filesystem.h>
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

