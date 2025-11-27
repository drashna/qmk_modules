// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2025 dmyoung <@dmyoung9>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Minimal WPM statistics tracking
 *
 * This is a lightweight alternative to typing_stats that only tracks:
 * - Current WPM
 * - Average WPM (exponential moving average)
 * - Maximum WPM
 *
 * No persistent storage, sessions, or complex statistics.
 */

// No configuration needed for this minimal implementation

/**
 * @brief WPM statistics structure
 */
typedef struct {
    uint16_t current_wpm;     /**< Current WPM reading */
    uint16_t average_wpm;     /**< Average WPM (exponential moving average) */
    uint16_t session_max_wpm; /**< Maximum WPM ever recorded */
} wpm_stats_t;

/**
 * @brief Configuration structure for WPM bar graph
 */
typedef struct {
    uint16_t x;      /**< X position of the bar */
    uint16_t y;      /**< Y position of the bar */
    uint16_t width;  /**< Width of the bar */
    uint16_t height; /**< Height of the bar */
} wpm_bar_config_t;

/**
 * @brief Initialize WPM statistics tracking
 *
 * Call this once during keyboard initialization.
 */
void wpm_stats_init(void);

/**
 * @brief Get current WPM statistics
 *
 * @param stats Pointer to structure to fill with current statistics
 * @return true if successful, false if not initialized
 */
bool wpm_stats_get(wpm_stats_t *stats);

/**
 * @brief Get current WPM
 *
 * @return Current WPM reading
 */
uint16_t wpm_stats_get_current(void);

/**
 * @brief Get average WPM
 *
 * @return Average WPM (exponential moving average)
 */
uint16_t wpm_stats_get_avg(void);

/**
 * @brief Get maximum WPM
 *
 * @return Maximum WPM ever recorded
 */
uint16_t wpm_stats_get_max(void);

#ifdef SPLIT_KEYBOARD
/**
 * @brief Get split keyboard WPM statistics
 *
 * @param max_wpm
 * @param wpm_sum
 * @param wpm_count
 * @return true
 * @return false
 */
bool wpm_stats_get_split(uint16_t *max_wpm, uint32_t *wpm_sum, uint16_t *wpm_count);

/**
 * @brief Set split keyboard WPM statistics
 *
 * @param max_wpm
 * @param wpm_sum
 * @param wpm_count
 * @return true
 * @return false
 */
bool wpm_stats_set_split(uint16_t *max_wpm, uint32_t *wpm_sum, uint16_t *wpm_count);
#endif
