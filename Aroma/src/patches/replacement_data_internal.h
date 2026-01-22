#pragma once
#include "../common.h"
#include <stdint.h>

typedef struct {
    const uint32_t offsets[NUM_REGIONS];
    const uint32_t original_value, replacement_value;
    uint32_t* replaced_data_pointer;
} ReplacementData;

/**
 * Applies a replacement given a pointer to a ReplacementData
 * @param replacement_data_p Pointer to ReplacementData
 * @param rpl_text_address Address of text section of RPL loaded in memory
 * @param region Region to pick offset (to add to text address)
 * @return true if successful
 */
bool apply_replacement(ReplacementData *replacement_data_p, uint32_t rpl_text_address, Region region);
/**
 * Undoes a replacement given a pointer to a ReplacementData
 * @param replacement_data_p Pointer to ReplacementData
 * @return true if successful
 */
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
