// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_layer.h
 * @brief Layer Analysis Package - Internal API
 *
 * This package provides layer usage tracking and analysis.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

// Layer tracking API
void     ts_layer_record_press(uint8_t layer);
uint32_t ts_layer_get_presses(uint8_t layer);

// Layer queries
int8_t ts_layer_find_most_used(uint8_t *layer_out, uint32_t *count_out);
int8_t ts_layer_find_least_used(uint8_t *layer_out, uint32_t *count_out, bool nonzero_only);

#if TS_ENABLE_LAYER_TIME
// Layer time tracking
void     ts_layer_init(void);
void     ts_layer_on_change(layer_state_t new_state);
uint32_t ts_layer_get_time_ms(uint8_t layer);
float    ts_layer_get_time_ratio(uint8_t layer);
#endif
