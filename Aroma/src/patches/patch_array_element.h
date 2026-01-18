#pragma once
#include "base_patch.h"
#include "base_patch_internal.h"
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
 * Retrieve the pointer to the BasePatch of a patch inside a PatchArrayElement
 * @param element_p Pointer to the PatchArrayElement
 * @return Pointer to the BasePatch
 */
BasePatch *get_base_patch_data_p (PatchArrayElement *element_p);

#define GENERATE_SIMPLE_REPLACEMENT_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, \
    original_instruction, replacement_instruction \
) ( \
    (PatchArrayElement) { \
        .patch_type = SIMPLE_REPLACEMENT_PATCH, \
        .simple_replacement_patch = (SimpleReplacementPatch) { \
            .base_patch_data = _GENERATE_BASE_PATCH_ENTRY( \
                (setting_id), (menu_text), (is_enabled_default), \
                (offset_jpn), (offset_usa), (offset_eur) \
            ), \
            .ORIGINAL_INSTRUCTION = (original_instruction), \
            .REPLACEMENT_INSTRUCTION = (replacement_instruction), \
            .patched_instruction_address = NULL \
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
            .base_patch_data = _GENERATE_BASE_PATCH_ENTRY( \
                (setting_id), (menu_text), (is_enabled_default), \
                (offset_jpn), (offset_usa), (offset_eur) \
            ), \
            .function_replacement_data_JPN = (function_replacement_data_t) { \
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
            .function_replacement_data_USA = (function_replacement_data_t) { \
                .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                .physicalAddr  = 0, /*Unused*/ \
                .virtualAddr   = 0, /*Unused*/ \
                .replaceAddr   = (uint32_t)&(replacement_function), \
                .replaceCall   = &(replace_call_variable), \
                .targetProcess = FP_TARGET_PROCESS_ALL, \
                .ReplaceInRPX  = { \
                    .targetTitleIds      = (uint64_t[]){(tid_usa)}, \
                    .targetTitleIdsCount = 1, \
                    .versionMin          = 0, /* Ignore version as WWHD was never updated anyway*/ \
                    .versionMax          = 0xFFFF, \
                    .executableName      = (executable_name), \
                    .textOffset          = (offset_usa), \
                    .functionName        = NULL /*Unused*/ \
                } \
            }, \
            .function_replacement_data_EUR = (function_replacement_data_t) { \
                .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                .physicalAddr  = 0, /*Unused*/ \
                .virtualAddr   = 0, /*Unused*/ \
                .replaceAddr   = (uint32_t)&(replacement_function), \
                .replaceCall   = &(replace_call_variable), \
                .targetProcess = FP_TARGET_PROCESS_ALL, \
                .ReplaceInRPX  = { \
                    .targetTitleIds      = (uint64_t[]){(tid_eur)}, \
                    .targetTitleIdsCount = 1, \
                    .versionMin          = 0, /* Ignore version as WWHD was never updated anyway*/ \
                    .versionMax          = 0xFFFF, \
                    .executableName      = (executable_name), \
                    .textOffset          = (offset_eur), \
                    .functionName        = NULL /*Unused*/ \
                } \
            }, \
        } \
    } \
)
