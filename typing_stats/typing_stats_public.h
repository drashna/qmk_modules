// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_public.h
 * @brief QMK Typing Statistics Module - Public API
 *
 * This is the main public API for the typing statistics module. It provides
 * a clean interface for tracking keyboard usage statistics with optional
 * advanced features.
 *
 * @author dmyoung9
 * @version 2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include QMK_KEYBOARD_H

// =============================================================================
// CORE CONFIGURATION
// =============================================================================

/**
 * @defgroup TS_Config Configuration Options
 * @brief Configuration macros for the typing statistics module
 * @{
 */

/** Maximum number of layers to track (default: 8) */
#ifndef TS_MAX_LAYERS
#    define TS_MAX_LAYERS 8
#endif

/** Seconds between automatic EEPROM flushes (default: 120) */
#ifndef TS_FLUSH_SECONDS
#    define TS_FLUSH_SECONDS 120
#endif

/** Key events between automatic EEPROM flushes (default: 2000) */
#ifndef TS_FLUSH_EVENTS
#    define TS_FLUSH_EVENTS 2000
#endif

/** Automatically start new session on each boot (default: enabled) */
#ifndef TS_AUTO_NEW_SESSION_ON_BOOT
#    define TS_AUTO_NEW_SESSION_ON_BOOT 1
#endif

/** @} */

/**
 * @defgroup TS_Features Feature Flags
 * @brief Optional feature enable/disable flags
 * @{
 */

/** Enable layer time tracking (requires more memory) */
#ifndef TS_ENABLE_LAYER_TIME
#    define TS_ENABLE_LAYER_TIME 0
#endif

/** Enable bigram (key sequence) statistics */
#ifndef TS_ENABLE_BIGRAM_STATS
#    define TS_ENABLE_BIGRAM_STATS 0
#endif

/** Enable WPM (Words Per Minute) tracking */
#ifndef TS_ENABLE_WPM_TRACKING
#    define TS_ENABLE_WPM_TRACKING 1
#endif

/** Enable EEPROM persistent storage */
#ifndef TS_ENABLE_EEPROM_STORAGE
#    define TS_ENABLE_EEPROM_STORAGE 1
#endif

/** Enable advanced analysis functions */
#ifndef TS_ENABLE_ADVANCED_ANALYSIS
#    define TS_ENABLE_ADVANCED_ANALYSIS 0
#endif

/** @} */

// =============================================================================
// CORE DATA TYPES
// =============================================================================

/**
 * @defgroup TS_Types Core Data Types
 * @brief Public data structures and enumerations
 * @{
 */

/**
 * @brief Hand identification for split keyboards
 */
typedef enum {
    TS_HAND_LEFT,   /**< Left hand key */
    TS_HAND_RIGHT,  /**< Right hand key */
    TS_HAND_UNKNOWN /**< Unknown or center key */
} ts_hand_t;

/**
 * @brief Comprehensive typing statistics summary
 *
 * This structure contains all the key metrics tracked by the module.
 * Use ts_get_summary() to populate this structure.
 */
typedef struct {
    uint32_t total_lifetime_presses; /**< Total keys pressed since first use */
    uint32_t session_presses;        /**< Keys pressed in current session */
#if TS_ENABLE_WPM_TRACKING
    uint16_t current_wpm;     /**< Current WPM reading */
    uint16_t avg_wpm;         /**< Average WPM (exponential moving average) */
    uint16_t max_wpm;         /**< Lifetime maximum WPM */
    uint16_t session_max_wpm; /**< Session maximum WPM */
#endif
    float    left_hand_ratio;     /**< Left hand usage ratio (0.0-1.0) */
    uint8_t  most_used_layer;     /**< Most frequently used layer */
    uint8_t  most_used_mod;       /**< Most frequently used modifier */
    uint16_t most_used_pos_index; /**< Most frequently used key position */
} ts_summary_t;

/** @} */

// =============================================================================
// CORE SYSTEM API
// =============================================================================

/**
 * @defgroup TS_System System Management
 * @brief Core system initialization and lifecycle functions
 * @{
 */

/**
 * @brief Initialize the typing statistics module
 *
 * Call this once during keyboard initialization. It will load existing
 * statistics from EEPROM and initialize all tracking systems.
 *
 * @note This function is idempotent - calling it multiple times is safe
 */
void ts_init(void);

/**
 * @brief Periodic task function - call every 10ms
 *
 * Handles automatic EEPROM flushing and internal housekeeping.
 * Call this from your keyboard's matrix_scan_user() or similar.
 */
void ts_task_10ms(void);

/**
 * @brief Record a key event
 *
 * Call this from process_record_user() to track key presses.
 *
 * @param record The QMK key record
 * @param keycode The processed keycode
 */
void ts_on_keyevent(keyrecord_t *record, uint16_t keycode);

/**
 * @brief Handle layer state changes
 *
 * Call this from layer_state_set_user() to track layer usage.
 *
 * @param new_state The new layer state
 * @return The layer state (pass-through)
 */
layer_state_t ts_on_layer_change(layer_state_t new_state);

/**
 * @brief Reset all statistics to defaults
 *
 * This will clear all tracked statistics and reset the module to
 * its initial state. Changes are immediately saved to EEPROM.
 *
 * @warning This action cannot be undone
 */
void ts_reset_all_stats(void);

/**
 * @brief Start a new typing session
 *
 * Resets session-specific statistics (session presses, session WPM, session time)
 * while preserving all lifetime statistics. This is useful for tracking
 * individual typing sessions.
 *
 * @note This function preserves:
 *   - Total lifetime presses
 *   - Lifetime maximum WPM
 *   - Average WPM (EMA)
 *   - All key position statistics
 *   - All layer and modifier statistics
 */
void ts_start_new_session(void);

/**
 * @brief EEPROM initialization callback
 *
 * Call this from eeconfig_init_user() to handle EEPROM resets.
 */
void ts_eeconfig_init_user(void);

/** @} */

// =============================================================================
// STATISTICS QUERY API
// =============================================================================

/**
 * @defgroup TS_Query Statistics Query Functions
 * @brief Functions to retrieve typing statistics
 * @{
 */

/**
 * @brief Get comprehensive statistics summary
 *
 * Populates a summary structure with all key metrics.
 *
 * @param summary Pointer to summary structure to populate
 * @return true if successful, false if module not initialized
 */
bool ts_get_summary(ts_summary_t *summary);

#if TS_ENABLE_WPM_TRACKING
/**
 * @brief Get current words per minute
 * @return Current WPM reading
 */
uint16_t ts_get_current_wpm(void);

/**
 * @brief Get average words per minute
 * @return Average WPM (exponential moving average)
 */
uint16_t ts_get_avg_wpm(void);

/**
 * @brief Get lifetime maximum words per minute
 * @return Maximum WPM ever recorded
 */
uint16_t ts_get_max_wpm(void);

/**
 * @brief Get session maximum words per minute
 * @return Maximum WPM in current session
 */
uint16_t ts_get_session_max_wpm(void);
#endif

/**
 * @brief Get total lifetime key presses
 * @return Total number of keys pressed since first use
 */
uint32_t ts_get_total_presses(void);

/**
 * @brief Get session key presses
 * @return Number of keys pressed in current session
 */
uint32_t ts_get_session_presses(void);

/**
 * @brief Get session duration in minutes
 * @return Minutes since session started
 */
uint32_t ts_get_session_time_minutes(void);

/**
 * @brief Get left hand usage ratio
 * @return Ratio of left hand usage (0.0 = all right, 1.0 = all left)
 */
float ts_get_left_hand_ratio(void);

/**
 * @brief Get right hand usage ratio
 * @return Ratio of right hand usage (0.0 = all left, 1.0 = all right)
 */
float ts_get_right_hand_ratio(void);

/** @} */

// =============================================================================
// POSITION ANALYSIS API
// =============================================================================

/**
 * @defgroup TS_Position Key Position Analysis
 * @brief Functions for analyzing key position usage
 * @{
 */

/**
 * @brief Get press count for a specific key position
 *
 * @param row Key matrix row
 * @param col Key matrix column
 * @return Number of times this position was pressed
 */
uint32_t ts_get_key_presses(uint8_t row, uint8_t col);

/**
 * @brief Find the most frequently used key position
 *
 * @param row_out Pointer to store the row of most used key (can be NULL)
 * @param col_out Pointer to store the column of most used key (can be NULL)
 * @param count_out Pointer to store the press count (can be NULL)
 * @return true if a key was found, false if no keys have been pressed
 */
bool ts_find_most_used_key(uint8_t *row_out, uint8_t *col_out, uint32_t *count_out);

/**
 * @brief Determine which hand a key position belongs to
 *
 * @param row Key matrix row
 * @param col Key matrix column
 * @return Hand classification for this position
 */
ts_hand_t ts_get_key_hand(uint8_t row, uint8_t col);

/** @} */

// =============================================================================
// LAYER ANALYSIS API
// =============================================================================

/**
 * @defgroup TS_Layer Layer Usage Analysis
 * @brief Functions for analyzing layer usage patterns
 * @{
 */

/**
 * @brief Get press count for a specific layer
 *
 * @param layer Layer number (0-based)
 * @return Number of key presses on this layer
 */
uint32_t ts_get_layer_presses(uint8_t layer);

/**
 * @brief Find the most frequently used layer
 *
 * @param layer_out Pointer to store the layer number (can be NULL)
 * @param count_out Pointer to store the press count (can be NULL)
 * @return true if a layer was found, false if no layers have been used
 */
bool ts_find_most_used_layer(uint8_t *layer_out, uint32_t *count_out);

#if TS_ENABLE_LAYER_TIME
/**
 * @brief Get time spent on a specific layer (if enabled)
 *
 * @param layer Layer number (0-based)
 * @return Milliseconds spent on this layer
 */
uint32_t ts_get_layer_time_ms(uint8_t layer);

/**
 * @brief Get time ratio for a specific layer (if enabled)
 *
 * @param layer Layer number (0-based)
 * @return Ratio of total time spent on this layer (0.0-1.0)
 */
float ts_get_layer_time_ratio(uint8_t layer);
#endif

/** @} */

// =============================================================================
// MODIFIER ANALYSIS API
// =============================================================================

/**
 * @defgroup TS_Modifier Modifier Key Analysis
 * @brief Functions for analyzing modifier key usage
 * @{
 */

/**
 * @brief Get press count for a specific modifier
 *
 * @param mod_index Modifier index (0=LCtrl, 1=LShift, 2=LAlt, 3=LGui, 4=RCtrl, 5=RShift, 6=RAlt, 7=RGui)
 * @return Number of times this modifier was used
 */
uint32_t ts_get_modifier_presses(uint8_t mod_index);

/**
 * @brief Find the most frequently used modifier
 *
 * @param mod_out Pointer to store the modifier index (can be NULL)
 * @param count_out Pointer to store the press count (can be NULL)
 * @return true if a modifier was found, false if no modifiers have been used
 */
bool ts_find_most_used_modifier(uint8_t *mod_out, uint32_t *count_out);

/**
 * @brief Get human-readable name for a modifier index
 *
 * @param mod_index Modifier index (0-7)
 * @return String name of the modifier (e.g., "LCtrl", "RShift")
 */
const char *ts_get_modifier_name(uint8_t mod_index);

/** @} */

// =============================================================================
// OPTIONAL FEATURES API
// =============================================================================

#if TS_ENABLE_BIGRAM_STATS
/**
 * @defgroup TS_Bigram Bigram Analysis (Optional)
 * @brief Functions for analyzing key sequence patterns
 * @{
 */

/** Maximum number of bigrams tracked */
#    define TS_MAX_BIGRAMS 64

/**
 * @brief Bigram (key sequence) data structure
 */
typedef struct {
    uint8_t  key1_pos; /**< First key position index */
    uint8_t  key2_pos; /**< Second key position index */
    uint16_t count;    /**< Number of times this sequence occurred */
} ts_bigram_t;

/**
 * @brief Find the most common key sequence
 *
 * @param pos1_out Pointer to store first key position (can be NULL)
 * @param pos2_out Pointer to store second key position (can be NULL)
 * @param count_out Pointer to store the sequence count (can be NULL)
 * @return true if a sequence was found, false otherwise
 */
bool ts_find_most_common_bigram(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out);

/**
 * @brief Get top bigrams
 *
 * @param output Array to store bigram data
 * @param max_count Maximum number of bigrams to return
 * @param actual_count Pointer to store actual number returned
 */
void ts_get_top_bigrams(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count);

/**
 * @brief Reset all bigram statistics
 */
void ts_reset_bigram_stats(void);

/** @} */
#endif

#if TS_ENABLE_ADVANCED_ANALYSIS
/**
 * @defgroup TS_Analysis Advanced Analysis (Optional)
 * @brief Advanced typing pattern analysis functions
 * @{
 */

/**
 * @brief Calculate key usage entropy
 *
 * Higher entropy indicates more even key usage distribution.
 *
 * @return Entropy value (higher = more even distribution)
 */
float ts_calculate_key_entropy(void);

/**
 * @brief Get estimated same-finger key presses
 *
 * @return Estimated number of consecutive same-finger presses
 */
uint32_t ts_get_same_finger_presses(void);

/**
 * @brief Get estimated finger roll count
 *
 * @return Estimated number of finger roll sequences
 */
uint32_t ts_get_finger_rolls(void);

/**
 * @brief Calculate hand balance score
 *
 * @return Score from 0.0 (poor balance) to 1.0 (perfect balance)
 */
float ts_calculate_hand_balance_score(void);

/**
 * @brief Calculate finger balance score
 *
 * @return Score from 0.0 (poor balance) to 1.0 (perfect balance)
 */
float ts_calculate_finger_balance_score(void);

/**
 * @brief Count alternating hand sequences
 *
 * @return Number of left-right-left or right-left-right sequences
 */
uint32_t ts_count_alternating_hands(void);

/**
 * @brief Calculate typing rhythm variance
 *
 * @return Variance in typing rhythm (lower = more consistent)
 */
float ts_calculate_typing_rhythm_variance(void);

/** @} */
#endif

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @defgroup TS_Utility Utility Functions
 * @brief Helper functions for working with the module
 * @{
 */

/**
 * @brief Check if the module is initialized
 *
 * @return true if initialized, false otherwise
 */
bool ts_is_initialized(void);

/**
 * @brief Convert row/column to position index
 *
 * @param row Key matrix row
 * @param col Key matrix column
 * @return Position index for internal use
 */
uint16_t ts_pos_to_index(uint8_t row, uint8_t col);

/**
 * @brief Convert position index to row/column
 *
 * @param index Position index
 * @param row_out Pointer to store row
 * @param col_out Pointer to store column
 */
void ts_index_to_pos(uint16_t index, uint8_t *row_out, uint8_t *col_out);

/**
 * @brief Determine which hand a key position belongs to
 *
 * @param row Key matrix row
 * @param col Key matrix column
 * @return Hand classification for this position
 */
ts_hand_t ts_pos_to_hand(uint8_t row, uint8_t col);

/** @} */
