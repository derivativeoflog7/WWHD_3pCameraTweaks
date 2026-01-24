/*  Copyright 2023 Pretendo Network contributors <pretendo.network>
 *  Copyright 2023 Ash Logan <ash@heyquark.com>
 *  Copyright 2019 Maschell
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified/adapted by derivativeoflog7, 2026
 *
 * Original code:
 * https://github.com/PretendoNetwork/Inkay/blob/fd06e2f2aaa8561f5379666d3af6c5781c56cef2/src/utils/replace_mem.cpp
 */

#include "../utils/logger.h"
#include "../common.h"
#include "setting_entry.h"
#include "setting_entry_internal.h"
#include "patch_strings_internal.h"
#include "replacement_data_internal.h"
#include "simple_replacement_patch.h"
#include "simple_replacement_patch_internal.h"
#include <coreinit/cache.h>
#include <coreinit/memorymap.h>
#include <kernel/kernel.h>
#include <stdbool.h>
#include <stdint.h>

bool apply_simple_replacement_patch(SimpleReplacementPatch *patch_p, uint32_t rpl_text_address, Region region) {
    SettingEntry *setting_entry_p = &patch_p->setting_entry;

    if (already_done(*setting_entry_p))
        return true;

    char *debug_prefix;
    bool res;

    if (setting_entry_p->is_enabled) {
        // Apply
        debug_prefix = APPLY;
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH, APPLY, setting_entry_p->setting_id);
        res = apply_replacement(&patch_p->replacement_data, rpl_text_address, region);
        if (res)
            setting_entry_p->is_applied = true;
    } else {
        // Undo
        debug_prefix = UNDO;
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH, UNDO, setting_entry_p->setting_id);
        res = undo_replacement(&patch_p->replacement_data);
        if (res)
            setting_entry_p->is_applied = false;
    }

    if (res) {
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_PATCH_SUCCEEDED, debug_prefix, setting_entry_p->setting_id);
        return true;
    } else {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_PATCH_FAILED, debug_prefix, setting_entry_p->setting_id);
        return false;
    }
}

bool force_undo_simple_replacement_patch(SimpleReplacementPatch *patch_p) {
    SettingEntry *setting_entry_p = &patch_p->setting_entry;

    if (!setting_entry_p->is_applied) {
        DEBUG_FUNCTION_LINE_VERBOSE(DEBUG_MESSAGE_NOT_APPLIED, setting_entry_p->setting_id);
        return true;
    }

    DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_DOING_SIMPLE_REPLACEMENT_PATCH, UNDO, setting_entry_p->setting_id);

    if (undo_replacement(&patch_p->replacement_data)) {
        DEBUG_FUNCTION_LINE_INFO(DEBUG_MESSAGE_PATCH_SUCCEEDED, UNDO, setting_entry_p->setting_id);
        setting_entry_p->is_applied = false;
        return true;
    } else {
        DEBUG_FUNCTION_LINE_ERR(DEBUG_MESSAGE_PATCH_FAILED, UNDO, setting_entry_p->setting_id);
        return false;
    }
}

