#pragma once
#include "../common.h"
#include <stdint.h>

typedef struct {
    const uint32_t offsets[NUM_REGIONS];
    const uint32_t original_value, replacement_value;
    uint32_t* replaced_data_pointer;
} ReplacementData;

bool apply_replacement(ReplacementData *replacement_data_p, uint32_t rpl_text_address, Region region);
bool undo_replacement(ReplacementData *replacement_data_p);

#define _GENERATE_REPLACEMENT_DATA( \
    offset_jpn, offset_usa, offset_eur, \
    original_value_, replacement_value_ \
) ( \
    (ReplacementData){ \
        .offsets[JPN] = (offset_jpn), \
        .offsets[USA] = (offset_usa), \
        .offsets[EUR] = (offset_eur), \
        .original_value = (original_value_), \
        .replacement_value = (replacement_value_), \
        .replaced_data_pointer = NULL \
    } \
)
