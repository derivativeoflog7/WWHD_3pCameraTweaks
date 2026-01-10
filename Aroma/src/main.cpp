#include "common.h"
#include "patch.h"
#include "utils/logger.h"
#include "settings.h"

#include <wups.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemStub.h>
#include <wups/config_api.h>

#include <forward_list>

#include <malloc.h>
WUPS_PLUGIN_NAME(PLUGIN_NAME);
WUPS_PLUGIN_DESCRIPTION("Invert the Y axis of the third person camera in TLOZ: The Wind Waker HD");
WUPS_PLUGIN_VERSION(PLUGIN_VERSION);
WUPS_PLUGIN_AUTHOR("derivativeoflog7");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE(STORAGE_PLUGIN_ID);

INITIALIZE_PLUGIN() {
    initLogging();
    initConfig();
    deinitLogging();
}

DEINITIALIZE_PLUGIN() {
    // Unpatch if previously patched
    if (patch_result == SUCCESS)
        patch(true);
    deinitLogging();
}

ON_APPLICATION_START() {
    initLogging();
    patch_result = UNINITIALIZED;

    // Try patching if enabled
    if (is_enabled)
        patch_result = patch(false);
    else
        patch_result = DISABLED;
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

