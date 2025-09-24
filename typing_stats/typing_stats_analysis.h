// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_analysis.h
 * @brief Advanced Analysis Package - Internal API
 *
 * This package provides advanced typing pattern analysis functions.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

// Advanced analysis functions
float    ts_analysis_calculate_key_entropy(void);
uint32_t ts_analysis_estimate_same_finger_presses(void);
uint32_t ts_analysis_estimate_finger_rolls(void);

// Summary generation
void ts_analysis_get_summary(ts_summary_t *summary);

#if TS_ENABLE_ADVANCED_ANALYSIS
// Extended analysis (optional)
float    ts_analysis_calculate_hand_balance_score(void);
float    ts_analysis_calculate_finger_balance_score(void);
uint32_t ts_analysis_count_alternating_hands(void);
float    ts_analysis_calculate_typing_rhythm_variance(void);
#endif
