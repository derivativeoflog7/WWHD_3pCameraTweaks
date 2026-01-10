/* Based on:
 * https://github.com/wiiu-env/WiiUPluginSystem/blob/a789f3234e95795cc6578e5ba66dea0fa1e0c7c3/plugins/example_plugin_cpp/src/main.cpp
 */

#include "settings.h"
#include "common.h"
#include "patch.h"
#include "utils/logger.h"
#include <wups/config/WUPSConfigItemStub.h>
#include <coreinit/filesystem.h>
#include <wups.h>
#include <format>

void initConfig() {
    WUPSConfigAPIOptionsV1 config_options = {.name = PLUGIN_NAME};
    if (WUPSConfigAPI_Init(config_options, ConfigMenuOpenedCallback, ConfigMenuClosedCallback) != WUPSCONFIG_API_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to init config api");
    }

    WUPSStorageError storage_res;
    if ((storage_res = WUPSStorageAPI::GetOrStoreDefault(ENABLE_SETTING_ID, is_enabled, ENABLE_SETTING_DEFAULT)) != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("GetOrStoreDefault failed: %s (%d)", WUPSStorageAPI_GetStatusStr(storage_res), storage_res);
    }
    if ((storage_res = WUPSStorageAPI::SaveStorage()) != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("GetOrStoreDefault failed: %s (%d)", WUPSStorageAPI_GetStatusStr(storage_res), storage_res);
    }
}

void boolItemChanged(ConfigItemBoolean *item, bool new_value) {
    if (std::string_view(ENABLE_SETTING_ID) == item->identifier) {
        is_enabled = new_value;
        WUPSStorageAPI::Store(item->identifier, new_value);
    }
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root_handle) {
    std::string status_text = "Status: "; // Initialize status string
    WUPSConfigCategory root = WUPSConfigCategory(root_handle);

    try {
        root.add(WUPSConfigItemBoolean::Create(ENABLE_SETTING_ID, "Enable",
                                               ENABLE_SETTING_DEFAULT, is_enabled,
                                               boolItemChanged));

        // Append for all fail states
        if (patch_result >= FAIL_NO_RPL)
            status_text += "FAIL! ";

        switch (patch_result) {
            case SUCCESS:
                status_text += "patched successfully";
                break;
            case DISABLED:
                status_text += "disabled";
                break;
            case NOT_WWHD:
                status_text += "not currently in WWHD";
                break;
            case FAIL_NO_RPL:
                status_text += "Could not find executable!";
                break;
            case FAIL_BAD_VERSION:
                status_text += "Bad game version!";
                break;
            case FAIL_ORIGINAL_VALUE_MISMATCH:
                status_text += "Original value mismatch!";
                break;
            case FAIL_VALUE_NOT_UPDATED:
                status_text += "Value hasn't been updated!";
                break;
            case UNINITIALIZED:
                status_text += "uninitialized";
                break;
        }
        root.add(WUPSConfigItemStub::Create(status_text));
#ifdef DEBUG
        // These will print the last value from when patch() was run, or 0 if it hasn't been ran yet'
        root.add(WUPSConfigItemStub::Create(std::format("Offset %08x", goffset)));
        root.add(WUPSConfigItemStub::Create(std::format("Address %08x", gaddress)));
        root.add(WUPSConfigItemStub::Create(std::format("Original value %08x", goriginal)));
#endif
    } catch (std::exception &e) {
        DEBUG_FUNCTION_LINE_ERR("Creating config menu failed: %s", e.what());
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
    }

    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback() {
    // If disabled and status tells that the last patch was a success, undo it
    if (!is_enabled && patch_result == SUCCESS) {
        if (patch_result == SUCCESS)
            patch(true);
        patch_result = DISABLED;
    // If enabled and status tells that the last time was disabled, patch
    } else if (is_enabled && patch_result == DISABLED) {
        patch_result = patch();
    }
    WUPSStorageAPI::SaveStorage();
}
