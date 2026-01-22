#pragma once
#include "base_patch.h"
#include "replacement_data_internal.h"
#include <stdint.h>

typedef struct {
    BasePatch base_patch_data;
    ReplacementData replacement_data;
} SimpleReplacementPatch;
