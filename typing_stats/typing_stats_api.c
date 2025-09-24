// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file typing_stats_api.c
 * @brief Public API implementation for typing statistics module
 *
 * This file implements the public API functions defined in typing_stats_public.h
 * by wrapping the internal implementation with a clean, stable interface.
 */

#include "typing_stats_public.h"
#include "typing_stats_private.h"
#include "typing_stats_analysis.h"
#include "typing_stats_position.h"
#include "typing_stats_layer.h"
#include "typing_stats_modifier.h"

#if TS_ENABLE_BIGRAM_STATS
#    include "typing_stats_bigram.h"
#endif
#if TS_ENABLE_WPM_TRACKING
#    include "typing_stats_wpm.h"
#endif

// =============================================================================
// SYSTEM MANAGEMENT API
// =============================================================================

void ts_init(void) {
    // Delegate to internal core implementation
    extern void ts_init_internal(void);
    ts_init_internal();
}

void ts_task_10ms(void) {
    // Delegate to internal task implementation
    extern void ts_task_10ms_internal(void);
    ts_task_10ms_internal();
}

void ts_on_keyevent(keyrecord_t *record, uint16_t keycode) {
    // Delegate to internal keyevent handler
    extern void ts_on_keyevent_internal(keyrecord_t * record, uint16_t keycode);
    ts_on_keyevent_internal(record, keycode);
}

layer_state_t ts_on_layer_change(layer_state_t new_state) {
    // Delegate to internal layer change handler
    extern layer_state_t ts_on_layer_change_internal(layer_state_t new_state);
    return ts_on_layer_change_internal(new_state);
}

void ts_reset_all_stats(void) {
    ts_reset_defaults();

#if TS_ENABLE_EEPROM_STORAGE
    // Force immediate save
    extern void ts_storage_force_flush(void);
    ts_storage_force_flush();
#endif
}

void ts_start_new_session(void) {
    // Delegate to internal session start function
    extern void ts_start_new_session_internal(void);
    ts_start_new_session_internal();
}

void ts_eeconfig_init_user(void) {
    // Delegate to internal eeconfig handler
    extern void ts_eeconfig_init_user_internal(void);
    ts_eeconfig_init_user_internal();
}

// =============================================================================
// STATISTICS QUERY API
// =============================================================================

bool ts_get_summary(ts_summary_t *summary) {
    if (!summary || !ts_core_is_initialized()) {
        return false;
    }

    ts_analysis_get_summary(summary);
    return true;
}

#if TS_ENABLE_WPM_TRACKING
uint16_t ts_get_current_wpm(void) {
    if (!ts_is_initialized()) return 0;
    return ts_wpm_get_current();
}

uint16_t ts_get_avg_wpm(void) {
    if (!ts_is_initialized()) return 0;
    return ts_wpm_get_avg();
}

uint16_t ts_get_max_wpm(void) {
    if (!ts_is_initialized()) return 0;
    return ts_wpm_get_max();
}

uint16_t ts_get_session_max_wpm(void) {
    if (!ts_is_initialized()) return 0;
    return ts_wpm_get_session_max();
}
#endif

uint32_t ts_get_total_presses(void) {
    if (!ts_is_initialized()) return 0;
    ts_counters_t *counters = ts_core_get_counters();
    return counters->total_presses;
}

uint32_t ts_get_session_presses(void) {
    if (!ts_is_initialized()) return 0;
    ts_counters_t *counters = ts_core_get_counters();
    return counters->session_presses;
}

// Note: ts_get_session_time_minutes is implemented in core for backward compatibility

float ts_get_left_hand_ratio(void) {
    if (!ts_is_initialized()) return 0.0f;

    ts_counters_t *counters = ts_core_get_counters();
    uint32_t       total    = counters->left_hand_presses + counters->right_hand_presses;

    if (total == 0) return 0.0f;
    return (float)counters->left_hand_presses / (float)total;
}

float ts_get_right_hand_ratio(void) {
    return 1.0f - ts_get_left_hand_ratio();
}

// =============================================================================
// POSITION ANALYSIS API
// =============================================================================

uint32_t ts_get_key_presses(uint8_t row, uint8_t col) {
    return ts_pos_get_presses(row, col);
}

bool ts_find_most_used_key(uint8_t *row_out, uint8_t *col_out, uint32_t *count_out) {
    uint16_t index;
    uint32_t count;

    if (!ts_pos_find_most_used(&index, &count)) {
        return false;
    }

    if (row_out || col_out) {
        uint8_t row, col;
        ts_index_to_pos(index, &row, &col);
        if (row_out) *row_out = row;
        if (col_out) *col_out = col;
    }

    if (count_out) *count_out = count;
    return true;
}

ts_hand_t ts_get_key_hand(uint8_t row, uint8_t col) {
    extern ts_hand_t ts_pos_to_hand_internal(uint8_t row, uint8_t col);
    return ts_pos_to_hand_internal(row, col);
}

// Note: ts_pos_to_hand is implemented in core for backward compatibility

// =============================================================================
// LAYER ANALYSIS API
// =============================================================================

uint32_t ts_get_layer_presses(uint8_t layer) {
    return ts_layer_get_presses(layer);
}

bool ts_find_most_used_layer(uint8_t *layer_out, uint32_t *count_out) {
    uint8_t  layer;
    uint32_t count;

    if (ts_layer_find_most_used(&layer, &count) <= 0) {
        return false;
    }

    if (layer_out) *layer_out = layer;
    if (count_out) *count_out = count;
    return true;
}

#if TS_ENABLE_LAYER_TIME
uint32_t ts_get_layer_time_ms(uint8_t layer) {
    return ts_layer_get_time_ms(layer);
}

float ts_get_layer_time_ratio(uint8_t layer) {
    return ts_layer_get_time_ratio(layer);
}
#endif

// =============================================================================
// MODIFIER ANALYSIS API
// =============================================================================

uint32_t ts_get_modifier_presses(uint8_t mod_index) {
    return ts_mod_get_presses(mod_index);
}

bool ts_find_most_used_modifier(uint8_t *mod_out, uint32_t *count_out) {
    uint8_t  mod;
    uint32_t count;

    if (ts_mod_find_most_used(&mod, &count) <= 0) {
        return false;
    }

    if (mod_out) *mod_out = mod;
    if (count_out) *count_out = count;
    return true;
}

const char *ts_get_modifier_name(uint8_t mod_index) {
    return ts_mod_bit_to_string(mod_index);
}

// =============================================================================
// OPTIONAL FEATURES API
// =============================================================================

#if TS_ENABLE_BIGRAM_STATS
bool ts_find_most_common_bigram(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out) {
    return ts_bigram_find_most_used(pos1_out, pos2_out, count_out);
}

void ts_get_top_bigrams(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count) {
    ts_bigram_get_top(output, max_count, actual_count);
}

void ts_reset_bigram_stats(void) {
    ts_bigram_reset();
}
#endif

#if TS_ENABLE_ADVANCED_ANALYSIS
float ts_calculate_key_entropy(void) {
    return ts_analysis_calculate_key_entropy();
}

uint32_t ts_get_same_finger_presses(void) {
    return ts_analysis_estimate_same_finger_presses();
}

uint32_t ts_get_finger_rolls(void) {
    return ts_analysis_estimate_finger_rolls();
}

float ts_calculate_hand_balance_score(void) {
    return ts_analysis_calculate_hand_balance_score();
}

float ts_calculate_finger_balance_score(void) {
    return ts_analysis_calculate_finger_balance_score();
}

uint32_t ts_count_alternating_hands(void) {
    return ts_analysis_count_alternating_hands();
}

float ts_calculate_typing_rhythm_variance(void) {
    return ts_analysis_calculate_typing_rhythm_variance();
}
#endif

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

bool ts_is_initialized(void) {
    return ts_core_is_initialized();
}

uint16_t ts_pos_to_index(uint8_t row, uint8_t col) {
    // Delegate to internal utility
    extern uint16_t ts_pos_to_index_internal(uint8_t row, uint8_t col);
    return ts_pos_to_index_internal(row, col);
}

void ts_index_to_pos(uint16_t index, uint8_t *row_out, uint8_t *col_out) {
    // Delegate to internal utility
    extern void ts_index_to_pos_internal(uint16_t index, uint8_t *row_out, uint8_t *col_out);
    ts_index_to_pos_internal(index, row_out, col_out);
}
