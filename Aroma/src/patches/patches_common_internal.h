#pragma once
#include <stdint.h>

/**
 Retrieves the address of the text section a loaded RPL/RPX given its name
 @param name Name of the RPL/RPX to look for
 @return Address of the text section of the RPL/RPX, or 0xFFFFFFFF if not found/in case of error
 */
uint32_t get_rpl_text_address(const char name[]);
