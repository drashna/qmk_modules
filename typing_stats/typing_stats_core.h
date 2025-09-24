// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_core.h
 * @brief Core typing statistics - Internal API
 *
 * This header is for internal use by the typing statistics module.
 * External code should use typing_stats_public.h instead.
 *
 * @warning Do not include this header directly in external code
 */

#include "typing_stats_private.h"

// Legacy compatibility - these functions are now internal
// External code should use typing_stats_public.h

// Internal core functions - used by typing_stats_api.c
void          ts_init_internal(void);
void          ts_task_10ms_internal(void);
void          ts_on_keyevent_internal(keyrecord_t *record, uint16_t keycode);
layer_state_t ts_on_layer_change_internal(layer_state_t new_state);
void          ts_eeconfig_init_user_internal(void);
void          ts_start_new_session_internal(void);

// Internal utility functions
uint16_t  ts_pos_to_index_internal(uint8_t row, uint8_t col);
void      ts_index_to_pos_internal(uint16_t index, uint8_t *row_out, uint8_t *col_out);
ts_hand_t ts_pos_to_hand_internal(uint8_t row, uint8_t col);
ts_hand_t ts_pos_to_hand(uint8_t row, uint8_t col); // Legacy compatibility

// Legacy compatibility functions (for backward compatibility)
bool     ts_core_is_initialized(void);
uint32_t ts_core_get_event_counter(void);
uint32_t ts_get_session_time_minutes(void);

#if TS_ENABLE_BIGRAM_STATS
void    ts_core_bigram_clear(void);
uint8_t ts_core_bigram_count(void);
bool    ts_core_bigram_get(uint8_t idx, uint8_t *p1, uint8_t *p2, uint16_t *count);
void    ts_core_bigram_increment(uint8_t p1, uint8_t p2);
#endif

// ---- Layer counters (read/write) ----
uint32_t ts_core_layer_get_presses(uint8_t layer);
void     ts_core_layer_increment(uint8_t layer);

#if TS_ENABLE_LAYER_TIME
// ---- Layer time (read/accumulate) ----
uint32_t ts_core_layer_get_time_ms(uint8_t layer);
void     ts_core_layer_add_time(uint8_t layer, uint32_t delta_ms);
#endif

// ---- Modifier counters (read/write) ----
uint32_t ts_core_mod_get_presses(uint8_t mod_index);       // 0..7
void     ts_core_mod_increment(uint8_t mod_index);         // +1 and mark dirty
                                                           //
                                                           // // ---- Position counters (read/write by index) ----
uint32_t ts_core_pos_get_presses_by_index(uint16_t index); // 0..(MATRIX_ROWS*MATRIX_COLS-1)
void     ts_core_pos_increment_by_index(uint16_t index);   // saturates at UINT16_MAX

// ---- Hand counters (read/write) ----
void     ts_core_hand_increment(ts_hand_t hand);
uint32_t ts_core_left_presses(void);
uint32_t ts_core_right_presses(void);

// Provide storage with access to the full blob address (for save/load)
ts_blob_t *ts_core_get_blob_ptr(void);
