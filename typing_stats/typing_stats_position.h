// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_position.h
 * @brief Position Analysis Package - Internal API
 *
 * This package provides key position usage tracking and analysis.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

// Position tracking API
void     ts_pos_record_press(uint8_t row, uint8_t col);
uint32_t ts_pos_get_presses(uint8_t row, uint8_t col);

// Position queries
bool ts_pos_find_most_used(uint16_t *index_out, uint32_t *count_out);
bool ts_pos_find_least_used(uint16_t *index_out, uint32_t *count_out, bool nonzero_only);

// Hand balance functions
void     ts_pos_record_hand_press(ts_hand_t hand);
uint32_t ts_pos_get_left_hand_presses(void);
uint32_t ts_pos_get_right_hand_presses(void);
