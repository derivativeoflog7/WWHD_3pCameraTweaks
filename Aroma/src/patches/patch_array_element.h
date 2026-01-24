#pragma once
#include "setting_entry.h"
#include "setting_entry_internal.h"
#include "replacement_data_internal.h"
#include "simple_replacement_patch.h"
#include "jump_patch.h"
#include <stdbool.h>

typedef enum {
    SIMPLE_REPLACEMENT_PATCH,
    JUMP_PATCH
} PatchType;

typedef struct {
    const PatchType patch_type;
    union {
        SimpleReplacementPatch simple_replacement_patch;
        JumpPatch jump_patch;
    };
} PatchArrayElement;

/**
 * Retrieve the pointer to the SettingEntry of a patch inside a PatchArrayElement
 * @param element_p Pointer to the PatchArrayElement
 * @return Pointer to the SettingEntry
 */
SettingEntry *get_setting_entry_p (PatchArrayElement *element_p);

#define GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, \
    original_value, replacement_value \
) ( \
    (PatchArrayElement) { \
        .patch_type = SIMPLE_REPLACEMENT_PATCH, \
        .simple_replacement_patch = (SimpleReplacementPatch) { \
            .setting_entry = GENERATE_SETTING_ENTRY( \
                (setting_id), (menu_text), (is_enabled_default) \
            ), \
            .replacement_data = _GENERATE_REPLACEMENT_DATA( \
                (offset_jpn), (offset_usa), (offset_eur), \
                (original_value), (replacement_value) \
            ) \
        } \
    } \
)

// libfunctionpatcher requires having the TID(s) in the struct, so we have no choice but pass them to it
// This also makes the TID check by this plugin redundant for jump patches, oh well...
#define GENERATE_JUMP_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, executable_name, \
    tid_jpn, tid_usa, tid_eur, \
    replacement_function, \
    replace_call_variable \
) ( \
    (PatchArrayElement) { \
        .patch_type = JUMP_PATCH, \
        .jump_patch = (JumpPatch) { \
            .setting_entry = GENERATE_SETTING_ENTRY( \
                (setting_id), (menu_text), (is_enabled_default) \
            ), \
            .function_replacement_data = (function_replacement_data_t[]) { \
                [JPN] = (function_replacement_data_t) { \
                    .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                    .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                    .physicalAddr  = 0, /*Unused*/ \
                    .virtualAddr   = 0, /*Unused*/ \
                    .replaceAddr   = (uint32_t)&(replacement_function), /*Address of replacement function*/ \
                    .replaceCall   = &(replace_call_variable), /*Pointer to code containing original instruction + jump back to original code*/ \
                    .targetProcess = FP_TARGET_PROCESS_ALL, \
                    .ReplaceInRPX  = { \
                        .targetTitleIds      = (uint64_t[]){(tid_jpn)}, \
                        .targetTitleIdsCount = 1, \
                        .versionMin          = 0, /*Ignore version as WWHD was never updated anyway*/ \
                        .versionMax          = 0xFFFF, /*Same as above*/ \
                        .executableName      = (executable_name), \
                        .textOffset          = (offset_jpn), \
                        .functionName        = NULL /*Unused*/ \
                    } \
                }, \
                [USA] = (function_replacement_data_t) { \
                    .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                    .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                    .physicalAddr  = 0, \
                    .virtualAddr   = 0, \
                    .replaceAddr   = (uint32_t)&(replacement_function), \
                    .replaceCall   = &(replace_call_variable), \
                    .targetProcess = FP_TARGET_PROCESS_ALL, \
                    .ReplaceInRPX  = { \
                        .targetTitleIds      = (uint64_t[]){(tid_usa)}, \
                        .targetTitleIdsCount = 1, \
                        .versionMin          = 0, \
                        .versionMax          = 0xFFFF, \
                        .executableName      = (executable_name), \
                        .textOffset          = (offset_usa), \
                        .functionName        = NULL \
                    } \
                }, \
                [EUR] = (function_replacement_data_t) { \
                    .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                    .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                    .physicalAddr  = 0, \
                    .virtualAddr   = 0, \
                    .replaceAddr   = (uint32_t)&(replacement_function), \
                    .replaceCall   = &(replace_call_variable), \
                    .targetProcess = FP_TARGET_PROCESS_ALL, \
                    .ReplaceInRPX  = { \
                        .targetTitleIds      = (uint64_t[]){(tid_eur)}, \
                        .targetTitleIdsCount = 1, \
                        .versionMin          = 0, \
                        .versionMax          = 0xFFFF, \
                        .executableName      = (executable_name), \
                        .textOffset          = (offset_eur), \
                        .functionName        = NULL \
                    } \
                }, \
            } \
        } \
    } \
)
