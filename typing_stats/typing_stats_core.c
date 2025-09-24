// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_private.h"
#if TS_ENABLE_EEPROM_STORAGE
#    include "typing_stats_storage.h"
#endif
#include "typing_stats_position.h"
#include "typing_stats_layer.h"
#include "typing_stats_modifier.h"

#if TS_ENABLE_BIGRAM_STATS
#    include "typing_stats_bigram.h"
#endif
#if TS_ENABLE_WPM_TRACKING
#    include "typing_stats_wpm.h"
#endif

#include "timer.h"
#include <string.h>

// Global state
static ts_blob_t     g_blob;
static bool          g_initialized        = false;
static uint32_t      g_event_counter      = 0;
static layer_state_t g_layer_state_cached = 0;

// Internal functions
static void ts_start_new_session_internal_impl(void);
ts_hand_t   ts_pos_to_hand_internal(uint8_t row, uint8_t col);

// Internal core API (renamed to avoid conflicts with public API)
void ts_init_internal(void) {
    if (g_initialized) return;

#if TS_ENABLE_EEPROM_STORAGE
    // Load data from storage
    ts_storage_load(&g_blob);
#else
    // Initialize with defaults when storage is disabled
    ts_reset_defaults();
#endif

    g_layer_state_cached = layer_state;
    g_initialized        = true;

    // Initialize submodules
#if TS_ENABLE_WPM_TRACKING
    ts_wpm_init();
#endif
#if TS_ENABLE_LAYER_TIME
    ts_layer_init();
#endif
#if TS_ENABLE_BIGRAM_STATS
    ts_bigram_init();
#endif

    // Session management
#if TS_AUTO_NEW_SESSION_ON_BOOT
    // Always start new session on boot (preserves lifetime stats)
    ts_start_new_session_internal_impl();
#else
    // Start new session only if never started before
    if (g_blob.c.session_start_time == 0) {
        ts_start_new_session_internal_impl();
    }
#endif
}

void ts_task_10ms_internal(void) {
    if (!g_initialized) return;

#if TS_ENABLE_WPM_TRACKING
    // Update WPM tracking
    ts_wpm_task();
#endif

#if TS_ENABLE_EEPROM_STORAGE
    // Handle periodic flushing
    ts_storage_task();
#endif
}

void ts_on_keyevent_internal(keyrecord_t *record, uint16_t keycode) {
    if (!record->event.pressed || !g_initialized) return;

    // Update core counters
    g_blob.c.total_presses++;
    g_blob.c.session_presses++;

    ts_hand_t hand = ts_pos_to_hand_internal(record->event.key.row, record->event.key.col);
    if (hand == TS_HAND_LEFT)
        g_blob.c.left_hand_presses++;
    else if (hand == TS_HAND_RIGHT)
        g_blob.c.right_hand_presses++;

    // Delegate to submodules
    ts_pos_record_press(record->event.key.row, record->event.key.col);
    ts_layer_record_press(get_highest_layer(layer_state | default_layer_state));
    ts_mod_record_press(get_mods() | get_oneshot_mods());

#if TS_ENABLE_BIGRAM_STATS
    ts_bigram_record_press(record->event.key.row, record->event.key.col);
#endif

    ts_core_mark_dirty();
    g_event_counter++;
}

layer_state_t ts_on_layer_change_internal(layer_state_t new_state) {
    g_layer_state_cached = new_state;

#if TS_ENABLE_LAYER_TIME
    ts_layer_on_change(new_state);
#endif

    return new_state;
}

static void ts_start_new_session_internal_impl(void) {
    g_blob.c.session_presses    = 0;
    g_blob.c.session_start_time = timer_read32();
#if TS_ENABLE_WPM_TRACKING
    ts_wpm_reset_session();
#endif
    ts_core_mark_dirty();
}

// Public API wrapper for session management
void ts_start_new_session_internal(void) {
    if (!g_initialized) return;
    ts_start_new_session_internal_impl();
}

// Utility functions for public API
uint16_t ts_pos_to_index_internal(uint8_t row, uint8_t col) {
    return (uint16_t)(row * MATRIX_COLS + col);
}

void ts_index_to_pos_internal(uint16_t index, uint8_t *row_out, uint8_t *col_out) {
    if (row_out) *row_out = (uint8_t)(index / MATRIX_COLS);
    if (col_out) *col_out = (uint8_t)(index % MATRIX_COLS);
}

// Internal state access for submodules
ts_counters_t *ts_core_get_counters(void) {
    return &g_blob.c;
}

void ts_core_mark_dirty(void) {
#if TS_ENABLE_EEPROM_STORAGE
    ts_storage_mark_dirty();
#endif
    // When storage is disabled, this function does nothing
}

bool ts_core_is_initialized(void) {
    return g_initialized;
}

uint32_t ts_core_get_event_counter(void) {
    return g_event_counter;
}

void ts_core_increment_event_counter(void) {
    g_event_counter++;
}

// Legacy compatibility function
uint32_t ts_get_session_time_minutes_internal(void) {
    if (!g_initialized || g_blob.c.session_start_time == 0) return 0;
    uint32_t elapsed_ms = timer_read32() - g_blob.c.session_start_time;
    return elapsed_ms / (60 * 1000);
}

// Legacy wrapper for backward compatibility
uint32_t ts_get_session_time_minutes(void) {
    return ts_get_session_time_minutes_internal();
}

void ts_reset_defaults(void) {
    memset(&g_blob, 0, sizeof(g_blob));
    g_blob.magic   = TS_MAGIC;
    g_blob.version = TS_VERSION;
    g_initialized  = true;
    ts_core_mark_dirty();

#if TS_ENABLE_LAYER_TIME
    ts_layer_init();
#endif
#if TS_ENABLE_BIGRAM_STATS
    ts_core_bigram_clear();
#endif
}

void ts_eeconfig_init_user_internal(void) {
    ts_reset_defaults();
#if TS_ENABLE_EEPROM_STORAGE
    ts_storage_force_flush();
#endif
}

// Utility functions (kept for internal use)
ts_hand_t ts_pos_to_hand_internal(uint8_t row, uint8_t col) {
    if (col < MATRIX_COLS / 2) return TS_HAND_LEFT;
    if (col >= (MATRIX_COLS + 1) / 2) return TS_HAND_RIGHT;
    return TS_HAND_UNKNOWN;
}

// Legacy compatibility wrapper
ts_hand_t ts_pos_to_hand_legacy(uint8_t row, uint8_t col) {
    return ts_pos_to_hand_internal(row, col);
}

// Legacy wrapper for backward compatibility
ts_hand_t ts_pos_to_hand(uint8_t row, uint8_t col) {
    return ts_pos_to_hand_internal(row, col);
}

// Internal helper functions

uint32_t ts_core_get_pos_presses(uint16_t pos_index) {
    if (pos_index >= (MATRIX_ROWS * MATRIX_COLS)) return 0;
    return g_blob.c.pos[pos_index].presses;
}

uint32_t ts_core_get_consecutive_same_finger(void) {
    return g_blob.c.consecutive_same_finger;
}

uint32_t ts_core_get_finger_rolls(void) {
    return g_blob.c.finger_rolls;
}

#if TS_ENABLE_BIGRAM_STATS
void ts_core_bigram_clear(void) {
    g_blob.c.bigram_count = 0;
    memset(g_blob.c.bigrams, 0, sizeof(g_blob.c.bigrams));
    ts_core_mark_dirty();
}

uint8_t ts_core_bigram_count(void) {
    return g_blob.c.bigram_count;
}

bool ts_core_bigram_get(uint8_t idx, uint8_t *p1, uint8_t *p2, uint16_t *count) {
    if (idx >= g_blob.c.bigram_count) return false;
    if (p1) *p1 = g_blob.c.bigrams[idx].key1_pos;
    if (p2) *p2 = g_blob.c.bigrams[idx].key2_pos;
    if (count) *count = g_blob.c.bigrams[idx].count;
    return true;
}

void ts_core_bigram_increment(uint8_t p1, uint8_t p2) {
    for (uint8_t i = 0; i < g_blob.c.bigram_count; i++) {
        if (g_blob.c.bigrams[i].key1_pos == p1 && g_blob.c.bigrams[i].key2_pos == p2) {
            if (g_blob.c.bigrams[i].count != UINT16_MAX) {
                g_blob.c.bigrams[i].count++;
                ts_core_mark_dirty();
            }
            return;
        }
    }
    if (g_blob.c.bigram_count < TS_MAX_BIGRAMS) {
        uint8_t i                    = g_blob.c.bigram_count++;
        g_blob.c.bigrams[i].key1_pos = p1;
        g_blob.c.bigrams[i].key2_pos = p2;
        g_blob.c.bigrams[i].count    = 1;
        ts_core_mark_dirty();
    }
}
#endif

// ---- Layer counters ----
uint32_t ts_core_layer_get_presses(uint8_t layer) {
    if (layer >= TS_MAX_LAYERS) return 0;
    return g_blob.c.layer_counts[layer];
}

void ts_core_layer_increment(uint8_t layer) {
    if (layer >= TS_MAX_LAYERS) return;
    g_blob.c.layer_counts[layer]++;
    ts_core_mark_dirty();
}

#if TS_ENABLE_LAYER_TIME
uint32_t ts_core_layer_get_time_ms(uint8_t layer) {
    if (layer >= TS_MAX_LAYERS) return 0;
    return g_blob.c.layer_time_ms[layer];
}

void ts_core_layer_add_time(uint8_t layer, uint32_t delta_ms) {
    if (layer >= TS_MAX_LAYERS) return;
    g_blob.c.layer_time_ms[layer] += delta_ms;
    ts_core_mark_dirty();
}
#endif

uint32_t ts_core_mod_get_presses(uint8_t mod_index) {
    if (mod_index >= 8) return 0;
    return g_blob.c.mod_counts[mod_index];
}

void ts_core_mod_increment(uint8_t mod_index) {
    if (mod_index >= 8) return;
    g_blob.c.mod_counts[mod_index]++;
    ts_core_mark_dirty();
}

uint32_t ts_core_pos_get_presses_by_index(uint16_t index) {
    if (index >= (MATRIX_ROWS * MATRIX_COLS)) return 0;
    return g_blob.c.pos[index].presses;
}

void ts_core_pos_increment_by_index(uint16_t index) {
    if (index >= (MATRIX_ROWS * MATRIX_COLS)) return;
    if (g_blob.c.pos[index].presses != UINT16_MAX) {
        g_blob.c.pos[index].presses++;
        ts_core_mark_dirty();
    }
}

void ts_core_hand_increment(ts_hand_t hand) {
    if (hand == TS_HAND_LEFT) {
        g_blob.c.left_hand_presses++;
        ts_core_mark_dirty();
    } else if (hand == TS_HAND_RIGHT) {
        g_blob.c.right_hand_presses++;
        ts_core_mark_dirty();
    }
}

uint32_t ts_core_left_presses(void) {
    return g_blob.c.left_hand_presses;
}
uint32_t ts_core_right_presses(void) {
    return g_blob.c.right_hand_presses;
}

ts_blob_t *ts_core_get_blob_ptr(void) {
    return &g_blob;
}
