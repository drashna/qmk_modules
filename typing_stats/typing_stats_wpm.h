// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_wpm.h
 * @brief WPM Tracking Package - Internal API
 *
 * This package provides words-per-minute tracking and analysis.
 * External code should use the functions exposed in typing_stats_public.h
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

#if TS_ENABLE_WPM_TRACKING

// WPM tracking API
void ts_wpm_init(void);
void ts_wpm_task(void);
void ts_wpm_on_keypress(void);

// WPM data access
uint16_t ts_wpm_get_current(void);
uint16_t ts_wpm_get_avg(void);
uint16_t ts_wpm_get_max(void);
uint16_t ts_wpm_get_session_max(void);

// WPM data modification
void ts_wpm_update_max(uint16_t wpm);
void ts_wpm_update_session_max(uint16_t wpm);
void ts_wpm_update_ema(uint16_t wpm);
void ts_wpm_reset_session(void);

// Internal WPM state access
uint16_t ts_wpm_get_max_internal(void);
uint16_t ts_wpm_get_avg_ema_internal(void);
uint16_t ts_wpm_get_session_max_internal(void);
void     ts_wpm_set_max_internal(uint16_t wpm);
void     ts_wpm_set_avg_ema_internal(uint16_t wpm);
void     ts_wpm_set_session_max_internal(uint16_t wpm);

#endif // TS_ENABLE_WPM_TRACKING
