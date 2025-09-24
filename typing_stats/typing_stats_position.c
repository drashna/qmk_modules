// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_position.h"

#ifndef TS_POS_COUNT
#    define TS_POS_COUNT (MATRIX_ROWS * MATRIX_COLS)
#endif

void ts_pos_record_press(uint8_t row, uint8_t col) {
    if (!ts_core_is_initialized()) return;

    uint16_t index = ts_pos_to_index(row, col);

    // Record position press via accessor (saturating)
    ts_core_pos_increment_by_index(index);

    // Record hand balance
    extern ts_hand_t ts_pos_to_hand_internal(uint8_t row, uint8_t col);
    ts_hand_t        hand = ts_pos_to_hand_internal(row, col);
    ts_pos_record_hand_press(hand);
}

uint32_t ts_pos_get_presses(uint8_t row, uint8_t col) {
    if (!ts_core_is_initialized()) return 0;
    uint16_t index = ts_pos_to_index(row, col);
    return ts_core_pos_get_presses_by_index(index);
}

bool ts_pos_find_most_used(uint16_t *index_out, uint32_t *count_out) {
    if (!ts_core_is_initialized()) return false;

    uint32_t best_count = 0;
    uint16_t best_index = 0;
    bool     found      = false;

    for (uint16_t i = 0; i < TS_POS_COUNT; i++) {
        uint32_t presses = ts_core_pos_get_presses_by_index(i);
        if (presses > best_count) {
            best_count = presses;
            best_index = i;
            found      = true;
        }
    }

    if (found) {
        if (index_out) *index_out = best_index;
        if (count_out) *count_out = best_count;
    }
    return found;
}

bool ts_pos_find_least_used(uint16_t *index_out, uint32_t *count_out, bool nonzero_only) {
    if (!ts_core_is_initialized()) return false;

    uint32_t best_count = UINT32_MAX;
    uint16_t best_index = 0;
    bool     found      = false;

    for (uint16_t i = 0; i < TS_POS_COUNT; i++) {
        uint32_t presses = ts_core_pos_get_presses_by_index(i);
        if (nonzero_only && presses == 0) continue;
        if (presses < best_count) {
            best_count = presses;
            best_index = i;
            found      = true;
        }
    }

    if (found) {
        if (index_out) *index_out = best_index;
        if (count_out) *count_out = (best_count == UINT32_MAX ? 0 : best_count);
    }
    return found;
}

void ts_pos_record_hand_press(ts_hand_t hand) {
    if (!ts_core_is_initialized()) return;
    ts_core_hand_increment(hand);
}

uint32_t ts_pos_get_left_hand_presses(void) {
    if (!ts_core_is_initialized()) return 0;
    return ts_core_left_presses();
}

uint32_t ts_pos_get_right_hand_presses(void) {
    if (!ts_core_is_initialized()) return 0;
    return ts_core_right_presses();
}
