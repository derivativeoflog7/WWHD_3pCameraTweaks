/* Based on:
 * https://github.com/wiiu-env/WiiUPluginSystem/blob/a789f3234e95795cc6578e5ba66dea0fa1e0c7c3/plugins/example_plugin_cpp/src/main.cpp
 */

#include "patches/setting_entry.h"
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
    SettingEntry *setting_entry_p = NULL;

    for (int i = 0; i < game_patches.NUM_PATCHES; i++) {
        setting_entry_p = get_setting_entry_p(&game_patches.patches[i]);
        if (!strcmp(setting_entry_p->setting_id, item->identifier)) {
            setting_entry_p->is_enabled = new_val;
            break;
        }
    }

    // Guard if for some reason no element matches
    if (setting_entry_p == NULL)
        return;

    if (WUPSStorageAPI_StoreBool(NULL, setting_entry_p->setting_id, setting_entry_p->is_enabled)
        != WUPS_STORAGE_ERROR_SUCCESS)
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_STORAGE, setting_entry_p->setting_id);
    else
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_STORED, setting_entry_p->setting_id, setting_entry_p->is_enabled);

    config_changed = true;
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    const SettingEntry *setting_entry_p;

    // Add boolean settings for all patches
    for (int i = 0; i < game_patches.NUM_PATCHES; i++) {
        setting_entry_p = get_setting_entry_p(&game_patches.patches[i]);

        if (WUPSConfigItemBoolean_AddToCategory(
            root,
            setting_entry_p->setting_id,
            setting_entry_p->menu_text,
            setting_entry_p->is_enabled_default,
            setting_entry_p->is_enabled,
            &boolean_changed
        ) != WUPSCONFIG_API_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_ADD, setting_entry_p->setting_id);
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        } else
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_ADDED, setting_entry_p->setting_id);
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
        SettingEntry *setting_entry_p = get_setting_entry_p(&game_patches.patches[i]);
        if ((storageRes = WUPSStorageAPI_GetBool(NULL, setting_entry_p->setting_id, &setting_entry_p->is_enabled)) ==
            WUPS_STORAGE_ERROR_NOT_FOUND) {
            DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_BOOL_DEFAULT, setting_entry_p->setting_id);

            // Add the value to the storage if it's missing.
            if (WUPSStorageAPI_StoreBool(NULL, setting_entry_p->setting_id, setting_entry_p->is_enabled) != WUPS_STORAGE_ERROR_SUCCESS)
                DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_STORAGE, setting_entry_p->setting_id);
            else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS)
                DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_FAILED_BOOL_GET, setting_entry_p->setting_id);
            else
                DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_STORED, setting_entry_p->setting_id, setting_entry_p->is_enabled);
        } else
            DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_BOOL_GOT, setting_entry_p->setting_id, setting_entry_p->is_enabled);
    }

    DEBUG_FUNCTION_LINE_VERBOSE(DEBUV_MESSAGE_CONFIG_INIALIZED);
    WUPSStorageAPI_SaveStorage(false);
}
