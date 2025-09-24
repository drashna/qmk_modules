// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_public.h"

void keyboard_post_init_typing_stats(void) {
    ts_init();
}

void housekeeping_task_typing_stats(void) {
    ts_task_10ms();
}

bool process_record_typing_stats(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);
    return true;
}

layer_state_t layer_state_set_typing_stats(layer_state_t state) {
    return ts_on_layer_change(state);
}

void eeconfig_init_typing_stats(void) {
    ts_eeconfig_init_user();
}

// Get statistics
ts_summary_t summary;
if (ts_get_summary(&summary)) {
    printf("WPM: %d, Total: %lu\n", summary.current_wpm, summary.total_lifetime_presses);
}
