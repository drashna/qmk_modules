// Copyright 2025 drashna
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdint.h>
#include <stdbool.h>

// Supported debounce algorithms, mirroring quantum/debounce/*.c upstream implementations.
typedef enum {
    DYNAMIC_DEBOUNCE_SYM_DEFER_G = 0,
    DYNAMIC_DEBOUNCE_SYM_EAGER_PR,
    DYNAMIC_DEBOUNCE_SYM_DEFER_PR,
    DYNAMIC_DEBOUNCE_SYM_EAGER_PK,
    DYNAMIC_DEBOUNCE_SYM_DEFER_PK,
    DYNAMIC_DEBOUNCE_ASYM_EAGER_DEFER_PK,
    DYNAMIC_DEBOUNCE_NONE,
    DYNAMIC_DEBOUNCE_ALGO_COUNT,
} dynamic_debounce_algo_t;

#ifndef DYNAMIC_DEBOUNCE_DEFAULT_ALGO
#    define DYNAMIC_DEBOUNCE_DEFAULT_ALGO DYNAMIC_DEBOUNCE_SYM_DEFER_G
#endif

// Returns the currently active debounce algorithm.
dynamic_debounce_algo_t dynamic_debounce_get_algorithm(void);

// Switches to the given algorithm, resetting its internal state.
void dynamic_debounce_set_algorithm(dynamic_debounce_algo_t algo);

// Cycles forwards/backwards through the available algorithms.
void dynamic_debounce_next_algorithm(void);
void dynamic_debounce_previous_algorithm(void);

// Human readable name of the given algorithm, e.g. for display/console output.
const char *dynamic_debounce_get_algorithm_name(dynamic_debounce_algo_t algo);

// Returns the currently active debounce time, in milliseconds. 0 disables debouncing.
uint8_t dynamic_debounce_get_time(void);

// Changes the debounce time at runtime, resetting the active algorithm's internal state.
void dynamic_debounce_set_time(uint8_t time_ms);

// Raises/lowers the debounce time by the given step, clamping at 0/UINT8_MAX.
void dynamic_debounce_increase_time(uint8_t step);
void dynamic_debounce_decrease_time(uint8_t step);
