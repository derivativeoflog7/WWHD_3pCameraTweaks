#pragma once
#include "common.h"

enum PatchResult {
    UNINITIALIZED,
    SUCCESS,
    DISABLED,
    NOT_WWHD,
    FAIL_NO_RPL,
    FAIL_BAD_VERSION,
    FAIL_ORIGINAL_VALUE_MISMATCH,
    FAIL_VALUE_NOT_UPDATED,
};

inline PatchResult patch_result;

PatchResult patch(bool undo = false);
#define EXECUTABLE_NAME "cking.rpx"sv
