// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_analysis.h"
#include "typing_stats_position.h"
#include "typing_stats_layer.h"
#include "typing_stats_modifier.h"
#include <math.h>
#include <string.h>

#ifndef TS_POS_COUNT
#    define TS_POS_COUNT (MATRIX_ROWS * MATRIX_COLS)
#endif

// 1/ln(2) avoids log2f which may be missing on AVR libm
static inline float inv_ln2(void) {
    return 1.4426950408889634f;
}

float ts_analysis_calculate_key_entropy(void) {
    if (!ts_core_is_initialized()) return 0.0f;

    uint32_t total_presses = ts_get_total_presses();
    if (total_presses == 0) return 0.0f;

    float       entropy = 0.0f;
    const float K       = inv_ln2();
    for (uint16_t i = 0; i < TS_POS_COUNT; i++) {
        uint32_t n = ts_core_get_pos_presses(i);
        if (!n) continue;
        float p = (float)n / (float)total_presses;
        entropy -= p * (logf(p) * K);
    }
    return entropy;
}

uint32_t ts_analysis_estimate_same_finger_presses(void) {
    if (!ts_core_is_initialized()) return 0;
    return ts_core_get_consecutive_same_finger();
}

uint32_t ts_analysis_estimate_finger_rolls(void) {
    if (!ts_core_is_initialized()) return 0;
    return ts_core_get_finger_rolls();
}

void ts_analysis_get_summary(ts_summary_t *summary) {
    if (!summary || !ts_core_is_initialized()) return;

    memset(summary, 0, sizeof(*summary));

    // Basic statistics
    summary->total_lifetime_presses = ts_get_total_presses();
    summary->session_presses        = ts_get_session_presses();
#if TS_ENABLE_WPM_TRACKING
    summary->current_wpm     = ts_get_current_wpm();
    summary->avg_wpm         = ts_get_avg_wpm();
    summary->max_wpm         = ts_get_max_wpm();
    summary->session_max_wpm = ts_get_session_max_wpm();
#endif
    summary->left_hand_ratio = ts_get_left_hand_ratio();

    // Most used elements
    uint8_t layer;
    if (ts_layer_find_most_used(&layer, NULL) > 0) summary->most_used_layer = layer;

    uint8_t mod;
    if (ts_mod_find_most_used(&mod, NULL) > 0) summary->most_used_mod = mod;

    uint16_t pos;
    if (ts_pos_find_most_used(&pos, NULL) > 0) summary->most_used_pos_index = pos;
}

#if TS_ENABLE_ADVANCED_ANALYSIS
float ts_analysis_calculate_hand_balance_score(void) {
    float left_ratio = ts_get_left_hand_ratio();
    float deviation  = fabsf(left_ratio - 0.5f);
    return 1.0f - (deviation * 2.0f);
}

float ts_analysis_calculate_finger_balance_score(void) {
    return 0.5f;
}
uint32_t ts_analysis_count_alternating_hands(void) {
    return 0;
}
float ts_analysis_calculate_typing_rhythm_variance(void) {
    return 0.0f;
}
#endif
