#pragma once
#include "../common.h"
#include "base_patch.h"
#include <function_patcher/function_patching.h>

typedef struct {
    BasePatch base_patch_data;
    function_replacement_data_t function_replacement_data[NUM_REGIONS];
    PatchedFunctionHandle patched_function_handle;
} JumpPatch;
