// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_private.h
 * @brief Internal/Private API for typing statistics module
 *
 * This header contains internal functions and data structures that should
 * not be used directly by external code. It's used by the module's internal
 * implementation files.
 *
 * @warning Do not include this header in external code
 */

#include "typing_stats_public.h"

// =============================================================================
// INTERNAL CONFIGURATION
// =============================================================================

/** Magic number for EEPROM validation */
#define TS_MAGIC 0x54535432u // "TST2"

/** Data structure version */
#define TS_VERSION 0x0004

/** WPM exponential moving average alpha numerator */
#ifndef TS_WPM_EMA_ALPHA_NUM
#    define TS_WPM_EMA_ALPHA_NUM 1
#endif

/** WPM exponential moving average alpha denominator */
#ifndef TS_WPM_EMA_ALPHA_DEN
#    define TS_WPM_EMA_ALPHA_DEN 8
#endif

// =============================================================================
// INTERNAL DATA STRUCTURES
// =============================================================================

/**
 * @brief Per-position press counter
 */
typedef struct {
    uint16_t presses;
} ts_pos_t;

/**
 * @brief Core statistics counters
 */
typedef struct {
    uint32_t total_presses;
#if TS_ENABLE_WPM_TRACKING
    uint16_t max_wpm;
    uint16_t avg_wpm_ema;
    uint16_t session_max_wpm;
#endif
    uint32_t session_presses;
    uint32_t session_start_time;
    uint32_t left_hand_presses;
    uint32_t right_hand_presses;
    uint32_t consecutive_same_finger;
    uint32_t finger_rolls;

    ts_pos_t pos[MATRIX_ROWS * MATRIX_COLS];
    uint32_t mod_counts[8];
    uint32_t layer_counts[TS_MAX_LAYERS];

#if TS_ENABLE_LAYER_TIME
    uint32_t layer_time_ms[TS_MAX_LAYERS];
#endif

#if TS_ENABLE_BIGRAM_STATS
    ts_bigram_t bigrams[TS_MAX_BIGRAMS];
    uint8_t     bigram_count;
#endif
} ts_counters_t;

/**
 * @brief Complete data blob stored in EEPROM
 */
typedef struct {
    uint32_t      magic;
    uint16_t      version;
    uint32_t      crc32;
    ts_counters_t c;
} ts_blob_t;

// =============================================================================
// INTERNAL CORE API
// =============================================================================

/**
 * @brief Get pointer to internal counters structure
 * @return Pointer to counters (for internal use only)
 */
ts_counters_t *ts_core_get_counters(void);

/**
 * @brief Get pointer to complete data blob
 * @return Pointer to blob (for storage module)
 */
ts_blob_t *ts_core_get_blob_ptr(void);

/**
 * @brief Mark data as dirty (needs saving)
 */
void ts_core_mark_dirty(void);

/**
 * @brief Check if core is initialized
 * @return true if initialized
 */
bool ts_core_is_initialized(void);

/**
 * @brief Get internal event counter
 * @return Number of events processed
 */
uint32_t ts_core_get_event_counter(void);

/**
 * @brief Increment internal event counter
 */
void ts_core_increment_event_counter(void);

// =============================================================================
// INTERNAL POSITION API
// =============================================================================

/**
 * @brief Get press count by position index
 * @param pos_index Position index (0 to MATRIX_ROWS*MATRIX_COLS-1)
 * @return Press count
 */
uint32_t ts_core_get_pos_presses(uint16_t pos_index);

/**
 * @brief Increment press count by position index
 * @param pos_index Position index
 */
void ts_core_pos_increment_by_index(uint16_t pos_index);

/**
 * @brief Get press count by position index (alternative name)
 * @param index Position index
 * @return Press count
 */
uint32_t ts_core_pos_get_presses_by_index(uint16_t index);

/**
 * @brief Increment hand counter
 * @param hand Hand to increment
 */
void ts_core_hand_increment(ts_hand_t hand);

/**
 * @brief Get left hand press count
 * @return Left hand presses
 */
uint32_t ts_core_left_presses(void);

/**
 * @brief Get right hand press count
 * @return Right hand presses
 */
uint32_t ts_core_right_presses(void);

// =============================================================================
// INTERNAL LAYER API
// =============================================================================

/**
 * @brief Get layer press count
 * @param layer Layer number
 * @return Press count
 */
uint32_t ts_core_layer_get_presses(uint8_t layer);

/**
 * @brief Increment layer press count
 * @param layer Layer number
 */
void ts_core_layer_increment(uint8_t layer);

#if TS_ENABLE_LAYER_TIME
/**
 * @brief Get layer time in milliseconds
 * @param layer Layer number
 * @return Time in milliseconds
 */
uint32_t ts_core_layer_get_time_ms(uint8_t layer);

/**
 * @brief Add time to layer counter
 * @param layer Layer number
 * @param delta_ms Time to add in milliseconds
 */
void ts_core_layer_add_time(uint8_t layer, uint32_t delta_ms);
#endif

// =============================================================================
// INTERNAL MODIFIER API
// =============================================================================

/**
 * @brief Get modifier press count
 * @param mod_index Modifier index (0-7)
 * @return Press count
 */
uint32_t ts_core_mod_get_presses(uint8_t mod_index);

/**
 * @brief Increment modifier press count
 * @param mod_index Modifier index (0-7)
 */
void ts_core_mod_increment(uint8_t mod_index);

// =============================================================================
// INTERNAL BIGRAM API
// =============================================================================

#if TS_ENABLE_BIGRAM_STATS
/**
 * @brief Clear all bigram data
 */
void ts_core_bigram_clear(void);

/**
 * @brief Get number of tracked bigrams
 * @return Bigram count
 */
uint8_t ts_core_bigram_count(void);

/**
 * @brief Get bigram data by index
 * @param idx Bigram index
 * @param p1 Pointer to store first position
 * @param p2 Pointer to store second position
 * @param count Pointer to store count
 * @return true if valid index
 */
bool ts_core_bigram_get(uint8_t idx, uint8_t *p1, uint8_t *p2, uint16_t *count);

/**
 * @brief Increment bigram count
 * @param p1 First position
 * @param p2 Second position
 */
void ts_core_bigram_increment(uint8_t p1, uint8_t p2);
#endif

// =============================================================================
// INTERNAL ANALYSIS API
// =============================================================================

/**
 * @brief Get consecutive same finger count
 * @return Same finger press count
 */
uint32_t ts_core_get_consecutive_same_finger(void);

/**
 * @brief Get finger roll count
 * @return Finger roll count
 */
uint32_t ts_core_get_finger_rolls(void);

// =============================================================================
// INTERNAL UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Reset all statistics to defaults
 */
void ts_reset_defaults(void);

/**
 * @brief Position count for iteration
 */
#define TS_POS_COUNT (MATRIX_ROWS * MATRIX_COLS)
