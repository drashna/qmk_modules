// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_layer.h"
#include "timer.h"

#if TS_ENABLE_LAYER_TIME
static uint32_t g_layer_time_start = 0;
static uint8_t  g_current_layer    = 0;

static void ts_layer_update_time(void);
#endif

void ts_layer_record_press(uint8_t layer) {
    if (!ts_core_is_initialized() || layer >= TS_MAX_LAYERS) return;
    ts_core_layer_increment(layer);
}

uint32_t ts_layer_get_presses(uint8_t layer) {
    if (!ts_core_is_initialized() || layer >= TS_MAX_LAYERS) return 0;
    return ts_core_layer_get_presses(layer);
}

int8_t ts_layer_find_most_used(uint8_t *layer_out, uint32_t *count_out) {
    if (!ts_core_is_initialized()) return 0;

    uint32_t best_count = 0;
    uint8_t  best_layer = 0;
    bool     found      = false;

    for (uint8_t i = 0; i < TS_MAX_LAYERS; i++) {
        uint32_t presses = ts_core_layer_get_presses(i);
        if (presses > best_count) {
            best_count = presses;
            best_layer = i;
            found      = true;
        }
    }

    if (found) {
        if (layer_out) *layer_out = best_layer;
        if (count_out) *count_out = best_count;
        return 1;
    }
    return 0;
}

int8_t ts_layer_find_least_used(uint8_t *layer_out, uint32_t *count_out, bool nonzero_only) {
    if (!ts_core_is_initialized()) return 0;

    uint32_t best_count = UINT32_MAX;
    uint8_t  best_layer = 0;
    bool     found      = false;

    for (uint8_t i = 0; i < TS_MAX_LAYERS; i++) {
        uint32_t presses = ts_core_layer_get_presses(i);
        if (nonzero_only && presses == 0) continue;
        if (presses < best_count) {
            best_count = presses;
            best_layer = i;
            found      = true;
        }
    }

    if (found) {
        if (layer_out) *layer_out = best_layer;
        if (count_out) *count_out = (best_count == UINT32_MAX ? 0 : best_count);
        return 1;
    }
    return 0;
}

#if TS_ENABLE_LAYER_TIME
void ts_layer_init(void) {
    g_current_layer    = get_highest_layer(layer_state | default_layer_state);
    g_layer_time_start = timer_read32();
}

void ts_layer_on_change(layer_state_t new_state) {
    if (!ts_core_is_initialized()) return;
    ts_layer_update_time();
    g_current_layer = get_highest_layer(new_state | default_layer_state);
}

uint32_t ts_layer_get_time_ms(uint8_t layer) {
    if (!ts_core_is_initialized() || layer >= TS_MAX_LAYERS) return 0;
    return ts_core_layer_get_time_ms(layer);
}

float ts_layer_get_time_ratio(uint8_t layer) {
    if (!ts_core_is_initialized() || layer >= TS_MAX_LAYERS) return 0.0f;

    uint32_t total_time = 0;
    for (uint8_t i = 0; i < TS_MAX_LAYERS; i++) {
        total_time += ts_core_layer_get_time_ms(i);
    }
    if (total_time == 0) return 0.0f;

    return (float)ts_core_layer_get_time_ms(layer) / (float)total_time;
}

static void ts_layer_update_time(void) {
    if (!ts_core_is_initialized()) return;

    uint32_t now = timer_read32();
    if (g_layer_time_start != 0 && g_current_layer < TS_MAX_LAYERS) {
        uint32_t elapsed = now - g_layer_time_start;
        ts_core_layer_add_time(g_current_layer, elapsed);
    }
    g_layer_time_start = now;
}
#endif
