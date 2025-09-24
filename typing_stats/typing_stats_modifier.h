// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_modifier.h
 * @brief Modifier Analysis Package - Internal API
 *
 * This package provides modifier key usage tracking and analysis.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

// Modifier tracking API
void     ts_mod_record_press(uint8_t mods);
uint32_t ts_mod_get_presses(uint8_t modbit_index);

// Modifier queries
int8_t ts_mod_find_most_used(uint8_t *modbit_out, uint32_t *count_out);
int8_t ts_mod_find_least_used(uint8_t *modbit_out, uint32_t *count_out, bool nonzero_only);

// Modifier utilities
const char *ts_mod_bit_to_string(uint8_t modbit_index);
