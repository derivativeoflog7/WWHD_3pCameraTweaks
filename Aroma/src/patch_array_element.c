#include "patch_array_element.h"

inline BasePatch *get_base_patch_data_p (PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return &element_p->simple_replacement_patch.base_patch_data;
        case JUMP_PATCH:
            return &element_p->jump_patch.base_patch_data;
    }

    return NULL; //gcc complains otherwise
}

bool apply_patch_from_array(PatchArrayElement *element_p, uint32_t rpl_text_address, Region region) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return apply_simple_replacement_patch(&element_p->simple_replacement_patch, rpl_text_address, region);
        case JUMP_PATCH:
            return apply_jump_patch(&element_p->jump_patch, region);
    }

    return false; //gcc complains otherwise
}

bool force_undo_patch_from_array(PatchArrayElement *element_p) {
    switch (element_p->patch_type) {
        case SIMPLE_REPLACEMENT_PATCH:
            return force_undo_simple_replacement_patch(&element_p->simple_replacement_patch);
        case JUMP_PATCH:
            return force_undo_jump_patch(&element_p->jump_patch);
    }

    return false; //gcc complains otherwise
}

PatchArrayElement generate_jump_patch_array_element(
    const char *setting_id, const char *menu_text, const bool is_enabled_default,
    const uint32_t offset_jpn, const uint32_t offset_usa, const uint32_t offset_eur,
    const uint64_t tid_jpn, const uint64_t tid_usa, const uint64_t tid_eur,
    const char *executable_name, void *replacement_code_p
) {
    function_replacement_data_t function_replacement_data_JPN = {
        .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION,
        .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS,
        .physicalAddr  = 0,
        .virtualAddr   = 0,
        .targetProcess = FP_TARGET_PROCESS_ALL,
        .replaceAddr   = (uint32_t)replacement_code_p,
        .ReplaceInRPX  = {
            .targetTitleIds      = (const uint64_t[]){tid_jpn},
            .targetTitleIdsCount = 1,
            .executableName      = executable_name,
            .textOffset          = offset_jpn,
            .versionMin          = 0,
            .versionMax          = 0xFFFF,
            .functionName        = NULL
        }
    };
    function_replacement_data_t function_replacement_data_USA = {
        .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION,
        .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS,
        .physicalAddr  = 0,
        .virtualAddr   = 0,
        .targetProcess = FP_TARGET_PROCESS_ALL,
        .replaceAddr   = (uint32_t)replacement_code_p,
        .ReplaceInRPX  = {
            .targetTitleIds      = (const uint64_t[]){tid_usa},
            .targetTitleIdsCount = 1,
            .executableName      = executable_name,
            .textOffset          = offset_usa,
            .versionMin          = 0,
            .versionMax          = 0xFFFF,
            .functionName        = NULL
        }
    };
    function_replacement_data_t function_replacement_data_EUR = {
        .version       = FUNCTION_REPLACEMENT_DATA_STRUCT_VERSION,
        .type          = FUNCTION_PATCHER_REPLACE_FOR_EXECUTABLE_BY_ADDRESS,
        .physicalAddr  = 0,
        .virtualAddr   = 0,
        .targetProcess = FP_TARGET_PROCESS_ALL,
        .replaceAddr   = (uint32_t)replacement_code_p,
        .ReplaceInRPX  = {
            .targetTitleIds      = (const uint64_t[]){tid_eur},
            .targetTitleIdsCount = 1,
            .executableName      = executable_name,
            .textOffset          = offset_eur,
            .versionMin          = 0,
            .versionMax          = 0xFFFF,
            .functionName        = NULL
        }
    };
    JumpPatch patch = {
        .base_patch_data = _GENERATE_BASE_PATCH_ENTRY(
            setting_id, menu_text, is_enabled_default,
            offset_jpn, offset_usa, offset_eur
        ),
        .function_replacement_data_JPN = function_replacement_data_JPN,
        .function_replacement_data_USA = function_replacement_data_USA,
        .function_replacement_data_EUR = function_replacement_data_EUR,
    };
    patch.function_replacement_data_JPN.replaceCall = &patch.replace_call;
    patch.function_replacement_data_USA.replaceCall = &patch.replace_call;
    patch.function_replacement_data_EUR.replaceCall = &patch.replace_call;
    return (PatchArrayElement){
        .patch_type = JUMP_PATCH,
        .jump_patch = patch
    };
}
