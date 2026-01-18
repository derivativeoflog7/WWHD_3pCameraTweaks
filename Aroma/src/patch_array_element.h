#pragma once
#include "base_patch.h"
#include "simple_replacement_patch.h"
#include "jump_patch.h"

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

BasePatch *get_base_patch_data_p (PatchArrayElement *element_p);
bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region);
bool force_undo_patch_from_array(PatchArrayElement *element_p);

PatchArrayElement generate_jump_patch_array_element(
    const char *setting_id, const char *menu_text, const bool is_enabled_default,
    const uint32_t offset_jpn, const uint32_t offset_usa, const uint32_t offset_eur,
    const uint64_t tid_jpn, const uint64_t tid_usa, const uint64_t tid_eur,
    const char *executable_name, void *replacement_code_p
);



#define _GENERATE_BASE_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur \
) ( \
    (BasePatch){ \
        .SETTING_ID = (setting_id), \
        .MENU_TEXT = (menu_text), \
        .IS_ENABLED_DEFAULT = (is_enabled_default), \
        .OFFSET_JPN = (offset_jpn), \
        .OFFSET_USA = (offset_usa), \
        .OFFSET_EUR = (offset_eur), \
        .is_enabled = false, .is_applied = false \
    } \
)

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

#define GENERATE_JUMP_PATCH_ENTRY( \
    setting_id, menu_text, is_enabled_default, \
    offset_jpn, offset_usa, offset_eur, \
    tid_jpn, tid_usa, tid_eur, \
    exec_name, replacement_func_pointer \
) ( \
    (PatchArrayElement) { \
        .patch_type = JUMP_PATCH, \
        .jump_patch = { \
            .base_patch_data = _GENERATE_BASE_PATCH_ENTRY( \
                (setting_id), (menu_text), (is_enabled_default), \
                (offset_jpn), (offset_usa), (offset_eur) \
            ), \
            .function_replacement_data_JPN = { \
                .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                .physicalAddr  = 0, \
                .virtualAddr   = 0, \
                .targetProcess = FP_TARGET_PROCESS_ALL, \
                .replaceAddr = (uint32_t)replacement_func_pointer, \
                .ReplaceInRPX = { \
                    .targetTitleIds = (const uint64_t[]){(tid_jpn)}, \
                    .targetTitleIdsCount = 1, \
                    .executableName = (exec_name), \
                    .textOffset = (offset_jpn), \
                }, \
            }, \
            .function_replacement_data_USA = { \
                .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                .physicalAddr  = 0, \
                .virtualAddr   = 0, \
                .targetProcess = FP_TARGET_PROCESS_ALL, \
                .replaceAddr = (uint32_t)replacement_func_pointer, \
                .ReplaceInRPX = { \
                    .targetTitleIds = (const uint64_t[]){(tid_usa)}, \
                    .targetTitleIdsCount = 1, \
                    .executableName = (exec_name), \
                    .textOffset = (offset_usa), \
                }, \
            }, \
            .function_replacement_data_EUR = { \
                .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION, \
                .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS, \
                .physicalAddr  = 0, \
                .virtualAddr   = 0, \
                .targetProcess = FP_TARGET_PROCESS_ALL, \
                .replaceAddr = (uint32_t)replacement_func_pointer, \
                .ReplaceInRPX = { \
                    .targetTitleIds = (const uint64_t[]){(tid_eur)}, \
                    .targetTitleIdsCount = 1, \
                    .executableName = (exec_name), \
                    .textOffset = (offset_eur), \
                }, \
            }, \
            .replace_call = 0, \
        } \
    } \
)
