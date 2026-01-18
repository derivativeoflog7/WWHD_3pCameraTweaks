#pragma once

#define DEBUG_MESSAGE_LOOKING_FOR_RPL                       "Looking for %s"
#define DEBUG_MESSAGE_RPL_TEXT_ADDRESS                      "%s text address is at %08x"
#define DEBUG_MESSAGE_FAIL_GetNumberOfRPLs                  "OSDynLoad_GetNumberOfRPLs failed"
#define DEBUG_MESSAGE_FAIL_malloc                           "malloc failed"
#define DEBUG_MESSAGE_FAIL_GetRPLInfo                       "OSDynLoad_GetRPLInfo failed"
#define DEBUG_MESSAGE_FAIL_RPL_NOT_FOUND                    "RPL not found"

#define APPLY                                               "Applying"
#define UNDO                                                "Undoing"
#define DEBUG_MESSAGE_SUCCESS                               "%s %s succeeded"
#define FAIL                                                "%s %s FAILED! "
#define DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH        "%s simple replacement patch %s at address %08x"
#define DEBUG_MESSAGE_DOING_JUMP_PATCH                      "%s jump patch patch %s"
#define DEBUG_MESSAGE_RESETTING_IS_APPLIED                  "Resetting is_applied for %s"
#define DEBUG_MESSAGE_DISABLED_NOT_APPLIED                  "%s is disabled and not applied"
#define DEBUG_MESSAGE_UNDOING                               "%s is diabled, undoing"
#define DEBUG_MESSAGE_ENABLED_AND_APPLIED                   "%s is enabled and already applied"
#define DEBUG_MESSAGE_APPLYING                              "%s is enabled, applying"
#define DEBUG_MESSAGE_NOT_APPLIED                           "%s is not applied"

#define DEBUG_MESSAGE_ORIGINAL_VALUE_AT_ADDRESS             "%08x is originally %08x"
#define DEBUG_MESSAGE_PHYSICAL_ADDRESS                      "Physical address is %08x"
#define DEBUG_MESSAGE_NEW_VALUE_AT_ADDRESS                  "%08x is now %08x"
#define _MISMATCH                                           "; expected %08x found %08x"
#define DEBUG_MESSAGE_FAIL_ORIGINAL_VALUE_MISMATCH FAIL     "Original value mismatch" _MISMATCH
#define DEBUG_MESSAGE_FAIL_VALUE_NOT_UPDATED FAIL           "Value did not update" _MISMATCH
#define DEBUG_MESSAGE_FAIL_NULL_PATCHED_ADDRESS FAIL        "patched_instruction_address is NULL"

#define DEBUG_MESSAGE_FAIL_FUNCTION_PATCHER_ERROR FAIL      "FunctionPatcher returned %s"

