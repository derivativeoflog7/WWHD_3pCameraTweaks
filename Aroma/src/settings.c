/* Based on:
 * https://github.com/wiiu-env/WiiUPluginSystem/blob/a789f3234e95795cc6578e5ba66dea0fa1e0c7c3/plugins/example_plugin_cpp/src/main.cpp
 */

#include "patches/base_patch.h"
#include "patches/game_patches.h"
#include "settings.h"
#include <coreinit/title.h>
#include <coreinit/filesystem.h>
#include <wups.h>
#include <wups/storage.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemStub.h>

#define DEBUG_MESSAGE_FAILED_CONFIG_INIT     "Failed to init config"
#define DEBUV_MESSAGE_CONFIG_INIALIZED       "Config initialized"
#define DEBUG_MESSAGE_BOOL_DEFAULT           "Storing default bool setting for %s"
#define DEBUG_MESSAGE_FAILED_BOOL_STORAGE    "Failed to store bool %s"
#define DEBUG_MESSAGE_BOOL_STORED            "Stored bool %s with value %d"
#define DEBUG_MESSAGE_FAILED_BOOL_GET        "Failed to get bool %s"
#define DEBUG_MESSAGE_BOOL_GOT               "Got bool %s with value %d"
#define DEBUG_MESSAGE_FAILED_BOOL_ADD        "Failed to add bool entry for %s"
#define DEBUG_MESSAGE_BOOL_ADDED             "Addedd bool entry for %s"
#define SETTINGS_STATUS_STRING               "%s status: %s"

bool config_changed;

void boolean_changed(ConfigItemBoolean *item, bool new_val);
WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root);
void ConfigMenuClosedCallback();

void boolean_changed(ConfigItemBoolean *item, bool new_val) {
    BasePatch *base_patch_data_p = NULL;

    for (int i = 0; i < game_patches.NUM_PATCHES; i++) {
        base_patch_data_p = get_base_patch_data_p(&game_patches.patches[i]);
        if (!strcmp(base_patch_data_p->SETTING_ID, item->identifier)) {
            base_patch_data_p->is_enabled = new_val;
            break;
        }
    }

    // Guard if for some reason no element matches
    if (base_patch_data_p == NULL)
        return;

    if (WUPSStorageAPI_StoreBool(NULL, base_patch_data_p->SETTING_ID, base_patch_data_p->is_enabled)
        != WUPS_STORAGE_ERROR_SUCCESS)
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_STORAGE, base_patch_data_p->SETTING_ID);
    else
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_STORED, base_patch_data_p->SETTING_ID, base_patch_data_p->is_enabled);

    config_changed = true;
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    const BasePatch *base_patch_data_p;

    // Add boolean settings for all patches
    for (int i = 0; i < game_patches.NUM_PATCHES; i++) {
        base_patch_data_p = get_base_patch_data_p(&game_patches.patches[i]);

        if (WUPSConfigItemBoolean_AddToCategory(
            root,
            base_patch_data_p->SETTING_ID,
            base_patch_data_p->MENU_TEXT,
            base_patch_data_p->IS_ENABLED_DEFAULT,
            base_patch_data_p->is_enabled,
            &boolean_changed
        ) != WUPSCONFIG_API_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_ADD, base_patch_data_p->SETTING_ID);
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        } else
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_ADDED, base_patch_data_p->SETTING_ID);
    }

    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback() {
    if (!config_changed)
        return;

    apply_game_patches(&game_patches, OSGetTitleID());
    WUPSStorageAPI_SaveStorage(false);
    config_changed = false;
}

void initConfig() {
    WUPSConfigAPIOptionsV1 configOptions = {.name = PLUGIN_NAME};
    if (WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback) !=
        WUPSCONFIG_API_RESULT_SUCCESS)
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_CONFIG_INIT);


    WUPSStorageError storageRes;
    // Try to get value from storage
    for (int i = 0; i < game_patches.NUM_PATCHES; i++) {
        BasePatch *base_patch_data_p = get_base_patch_data_p(&game_patches.patches[i]);
        if ((storageRes = WUPSStorageAPI_GetBool(NULL, base_patch_data_p->SETTING_ID, &base_patch_data_p->is_enabled)) ==
            WUPS_STORAGE_ERROR_NOT_FOUND) {
            DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_BOOL_DEFAULT, base_patch_data_p->SETTING_ID);

            // Add the value to the storage if it's missing.
            if (WUPSStorageAPI_StoreBool(NULL, base_patch_data_p->SETTING_ID, base_patch_data_p->is_enabled) != WUPS_STORAGE_ERROR_SUCCESS)
                DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_STORAGE, base_patch_data_p->SETTING_ID);
            else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS)
                DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_GET, base_patch_data_p->SETTING_ID);
            else
                DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_STORED, base_patch_data_p->SETTING_ID, base_patch_data_p->is_enabled);
        } else
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_GOT, base_patch_data_p->SETTING_ID, base_patch_data_p->is_enabled);
    }

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUV_MESSAGE_CONFIG_INIALIZED);
    WUPSStorageAPI_SaveStorage(false);
}
