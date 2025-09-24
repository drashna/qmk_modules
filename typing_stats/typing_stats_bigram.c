// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_bigram.h"

#if TS_ENABLE_BIGRAM_STATS
#    include <string.h>

// Keep last key position (16-bit for full matrix index)
static uint16_t g_last_pos_index = 0xFFFF;

// Forward-declare the static helper before use
static void ts_bigram_record_sequence(uint16_t pos1, uint16_t pos2);

void ts_bigram_init(void) {
    g_last_pos_index = 0xFFFF;
}

void ts_bigram_record_press(uint8_t row, uint8_t col) {
    if (!ts_core_is_initialized()) return;
    uint16_t current_pos = ts_pos_to_index(row, col);

    if (g_last_pos_index != 0xFFFF && g_last_pos_index != current_pos) {
        ts_bigram_record_sequence(g_last_pos_index, current_pos);
    }
    g_last_pos_index = current_pos;
}

void ts_bigram_reset(void) {
    g_last_pos_index = 0xFFFF;
    if (!ts_core_is_initialized()) return;
    ts_core_bigram_clear();
}

// Store/update a bigram (pos1 -> pos2) using 8-bit indices
static void ts_bigram_record_sequence(uint16_t pos1, uint16_t pos2) {
    if (!ts_core_is_initialized() || pos1 == pos2) return;

    // NOTE: this compresses to 8-bit indices. If your matrix > 256
    // positions, consider switching the store to 16-bit.
    uint8_t p1 = (uint8_t)(pos1 & 0xFF);
    uint8_t p2 = (uint8_t)(pos2 & 0xFF);
    ts_core_bigram_increment(p1, p2);
}

bool ts_bigram_find_most_used(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out) {
    if (!ts_core_is_initialized()) return false;

    uint8_t  n          = ts_core_bigram_count();
    uint16_t best_count = 0;
    uint8_t  best_p1 = 0, best_p2 = 0;
    bool     found = false;

    for (uint8_t i = 0; i < n; i++) {
        uint8_t  p1, p2;
        uint16_t c;
        ts_core_bigram_get(i, &p1, &p2, &c);
        if (c > best_count) {
            best_count = c;
            best_p1    = p1;
            best_p2    = p2;
            found      = true;
        }
    }

    if (found) {
        if (pos1_out) *pos1_out = best_p1;
        if (pos2_out) *pos2_out = best_p2;
        if (count_out) *count_out = best_count;
    }
    return found;
}

void ts_bigram_get_top(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count) {
    if (!output || max_count == 0 || !ts_core_is_initialized()) {
        if (actual_count) *actual_count = 0;
        return;
    }

    uint8_t n                    = ts_core_bigram_count();
    uint8_t cap                  = (n < max_count) ? n : max_count;
    bool    used[TS_MAX_BIGRAMS] = {0};
    uint8_t out_n                = 0;

    // Greedy selection of highest counts without duplicates
    for (uint8_t pick = 0; pick < cap; pick++) {
        uint16_t best_count = 0;
        int8_t   best_idx   = -1;

        for (uint8_t i = 0; i < n; i++) {
            if (used[i]) continue;
            uint16_t c;
            ts_core_bigram_get(i, NULL, NULL, &c);
            if (c > best_count) {
                best_count = c;
                best_idx   = i;
            }
        }

        if (best_idx < 0 || best_count == 0) break;

        uint8_t  p1, p2;
        uint16_t c;
        ts_core_bigram_get((uint8_t)best_idx, &p1, &p2, &c);
        output[out_n].key1_pos = p1;
        output[out_n].key2_pos = p2;
        output[out_n].count    = c;
        used[best_idx]         = true;
        out_n++;
    }

    if (actual_count) *actual_count = out_n;
}

#endif // TS_ENABLE_BIGRAM_STATS
