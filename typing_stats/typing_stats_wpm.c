// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file typing_stats_wpm.c
 * @brief WPM Tracking Package Implementation
 *
 * This file implements words-per-minute tracking functionality as an optional
 * package that can be enabled/disabled via TS_ENABLE_WPM_TRACKING.
 */

#include "typing_stats_wpm.h"

#if TS_ENABLE_WPM_TRACKING

#    include "timer.h"

// =============================================================================
// WPM TRACKING IMPLEMENTATION
// =============================================================================

void ts_wpm_init(void) {
    // WPM initialization is handled by QMK's built-in WPM system
    // No additional initialization needed
}

void ts_wpm_task(void) {
    // Update WPM tracking ~every 50-100ms
    static uint32_t last = 0;
    uint32_t        now  = timer_read32();
    if (now - last >= 50) {
        last         = now;
        uint16_t wpm = get_current_wpm();

        // Track max WPM
        uint16_t current_max = ts_wpm_get_max_internal();
        if (wpm > current_max) {
            ts_wpm_set_max_internal(wpm);
            extern void ts_core_mark_dirty(void);
            ts_core_mark_dirty();
        }

        // Track session max WPM
        uint16_t current_session_max = ts_wpm_get_session_max_internal();
        if (wpm > current_session_max) {
            ts_wpm_set_session_max_internal(wpm);
            extern void ts_core_mark_dirty(void);
            ts_core_mark_dirty();
        }

        // Update EMA
        ts_wpm_update_ema(wpm);
    }
}

void ts_wpm_on_keypress(void) {
    // WPM tracking is handled automatically by QMK's built-in system
    // No additional processing needed per keypress
}

// =============================================================================
// WPM DATA ACCESS
// =============================================================================

uint16_t ts_wpm_get_current(void) {
    return get_current_wpm();
}

uint16_t ts_wpm_get_avg(void) {
    return ts_wpm_get_avg_ema_internal();
}

uint16_t ts_wpm_get_max(void) {
    return ts_wpm_get_max_internal();
}

uint16_t ts_wpm_get_session_max(void) {
    return ts_wpm_get_session_max_internal();
}

// =============================================================================
// WPM DATA MODIFICATION
// =============================================================================

void ts_wpm_update_max(uint16_t wpm) {
    uint16_t current_max = ts_wpm_get_max_internal();
    if (wpm > current_max) {
        ts_wpm_set_max_internal(wpm);
        extern void ts_core_mark_dirty(void);
        ts_core_mark_dirty();
    }
}

void ts_wpm_update_session_max(uint16_t wpm) {
    uint16_t current_session_max = ts_wpm_get_session_max_internal();
    if (wpm > current_session_max) {
        ts_wpm_set_session_max_internal(wpm);
        extern void ts_core_mark_dirty(void);
        ts_core_mark_dirty();
    }
}

void ts_wpm_update_ema(uint16_t wpm) {
    uint16_t ema  = ts_wpm_get_avg_ema_internal();
    int16_t  diff = (int16_t)wpm - (int16_t)ema;
    ema += (int16_t)((TS_WPM_EMA_ALPHA_NUM * diff) / TS_WPM_EMA_ALPHA_DEN);

    if (ema != ts_wpm_get_avg_ema_internal()) {
        ts_wpm_set_avg_ema_internal(ema);
        extern void ts_core_mark_dirty(void);
        ts_core_mark_dirty();
    }
}

void ts_wpm_reset_session(void) {
    ts_wpm_set_session_max_internal(0);
    extern void ts_core_mark_dirty(void);
    ts_core_mark_dirty();
}

// =============================================================================
// INTERNAL WPM STATE ACCESS
// =============================================================================

uint16_t ts_wpm_get_max_internal(void) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    return counters->max_wpm;
}

uint16_t ts_wpm_get_avg_ema_internal(void) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    return counters->avg_wpm_ema;
}

uint16_t ts_wpm_get_session_max_internal(void) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    return counters->session_max_wpm;
}

void ts_wpm_set_max_internal(uint16_t wpm) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    counters->max_wpm              = wpm;
}

void ts_wpm_set_avg_ema_internal(uint16_t wpm) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    counters->avg_wpm_ema          = wpm;
}

void ts_wpm_set_session_max_internal(uint16_t wpm) {
    extern ts_counters_t *ts_core_get_counters(void);
    ts_counters_t        *counters = ts_core_get_counters();
    counters->session_max_wpm      = wpm;
}

#endif // TS_ENABLE_WPM_TRACKING
