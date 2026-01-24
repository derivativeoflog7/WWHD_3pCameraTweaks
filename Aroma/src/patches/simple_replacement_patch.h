#pragma once
#include "setting_entry.h"
#include "replacement_data_internal.h"
#include <stdint.h>

typedef struct {
    SettingEntry setting_entry;
    ReplacementData replacement_data;
} SimpleReplacementPatch;
