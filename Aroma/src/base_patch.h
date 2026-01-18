#pragma once
#include "common.h"
#include <stdbool.h>
#include <stdint.h>

#define APPLY                                               "Applying"
#define UNDO                                                "Undoing"
#define DEBUG_MESSAGE_SUCCESS                               "%s %s succeeded"
#define FAIL                                                "%s %s FAILED! "
#define DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH        "%s simple replacement patch %s at address %08x"
#define DEBUG_MESSAGE_DOING_JUMP_PATCH                      "%s simple replacement patch %s"
#define DEBUG_MESSAGE_RESETTING_IS_APPLIED                  "Resetting is_applied for %s"
#define DEBUG_MESSAGE_DISABLED_NOT_APPLIED                  "%s is disabled and not applied"
#define DEBUG_MESSAGE_UNDOING                               "%s is diabled, undoing"
#define DEBUG_MESSAGE_ENABLED_AND_APPLIED                   "%s is enabled and already applied"
#define DEBUG_MESSAGE_APPLYING                              "%s is enabled, applying"
#define DEBUG_MESSAGE_NOT_APPLIED                           "%s is not applied"

typedef enum {
    JPN, USA, EUR
} Region;

typedef struct {
    const char *SETTING_ID, *MENU_TEXT;
    const bool IS_ENABLED_DEFAULT;
    const uint32_t OFFSET_JPN, OFFSET_USA, OFFSET_EUR;
    bool is_enabled, is_applied;
} BasePatch;

bool already_done(BasePatch base_patch_data);
uint32_t get_rpl_text_address(const char name[]);
