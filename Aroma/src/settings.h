#pragma once
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config_api.h>
#include <cstdint>

#define STORAGE_PLUGIN_ID "WWHD_invert3pYaxis"
#define ENABLE_SETTING_ID "enable"
#define ENABLE_SETTING_DEFAULT true
inline bool is_enabled = ENABLE_SETTING_DEFAULT;;

inline uint32_t goffset = 0, gaddress = 0, goriginal = 0;

void initConfig();
void boolItemChanged(ConfigItemBoolean *item, bool newValue);
WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle);
void ConfigMenuClosedCallback();
