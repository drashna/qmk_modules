// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_bigram.h
 * @brief Bigram Analysis Package - Internal API
 *
 * This package provides key sequence (bigram) tracking and analysis.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

#if TS_ENABLE_BIGRAM_STATS

#    define TS_MAX_BIGRAMS 64

// Note: ts_bigram_t is defined in typing_stats_public.h to avoid conflicts

// Bigram tracking API
void ts_bigram_init(void);
void ts_bigram_record_press(uint8_t row, uint8_t col);
void ts_bigram_reset(void);

// Bigram queries
bool ts_bigram_find_most_used(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out);
void ts_bigram_get_top(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count);

#endif
