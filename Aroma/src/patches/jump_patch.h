#pragma once
#include "base_patch.h"
#include <function_patcher/function_patching.h>

typedef struct {
    BasePatch base_patch_data;
    function_replacement_data_t function_replacement_data_JPN,
    function_replacement_data_USA, function_replacement_data_EUR;
    uint32_t replace_call;
    PatchedFunctionHandle patched_function_handle;
} JumpPatch;
