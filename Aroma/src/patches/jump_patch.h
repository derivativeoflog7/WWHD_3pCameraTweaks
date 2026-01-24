#pragma once
#include "../common.h"
#include "setting_entry.h"
#include <function_patcher/function_patching.h>

typedef struct {
    SettingEntry setting_entry;
    function_replacement_data_t function_replacement_data[NUM_REGIONS];
    PatchedFunctionHandle patched_function_handle;
} JumpPatch;
