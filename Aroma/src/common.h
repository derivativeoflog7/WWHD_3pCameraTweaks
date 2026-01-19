#pragma once
#include "utils/logger.h"

#define PLUGIN_NAME "WWHD 3rd person camera tweaks"
#define PLUGIN_DESCRIPTION "3rd camera person tweaks for TLOZ: The Wind Waker HD"
#define PLUGIN_VERSION "v2.0"
#define PLUGIN_AUTOR "derivativeoflog7"
#define PLUGIN_LICENSE "GPLv3"
#define PLUGIN_CONFIG_ID "WWHD_3pCameraTweaks"

#define WWHD_EXECUTABLE_NAME "cking.rpx"

#define WWHD_TID_JPN 0x0005000010143400
#define WWHD_TID_USA 0x0005000010143500
#define WWHD_TID_EUR 0x0005000010143600

#define INVERT_3P_Y_AXIS_JPN_OFFSET 0x004F9DB0
#define INVERT_3P_Y_AXIS_USA_OFFSET 0x004F9DA8
#define INVERT_3P_Y_AXIS_EUR_OFFSET 0x004F9DAC

#define INVERT_3P_Y_AXIS_ORIGINAL_INSTRUCTION    0xFFA00890 // fmr  f29,f1
#define INVERT_3P_Y_AXIS_REPLACEMENT_INSTRUCTION 0xFFA00850 // fneg f29,f1

/*
 * We're actually replacing the instruction *after* the lfs that loads the right stick Y position
 * This is because we use FunctionPatcher to run custom code, then run the original instruction before ultimately jumping back to the game code
 * If we did that with the previously mentioned instruction, we would just undo all the work
 */
#define CONSISTENT_3P_Y_AXIS_JPN_OFFSET 0x005102F0
#define CONSISTENT_3P_Y_AXIS_USA_OFFSET 0x005102E8
#define CONSISTENT_3P_Y_AXIS_EUR_OFFSET 0x005102EC

#define CONSISTENT_3P_Y_AXIS_ORIGINAL_INSTRUCTION 0xC19F0174 // lfs f12, +0x174(r31)

typedef enum {
    JPN=0, USA, EUR, NUM_REGIONS
} Region;

