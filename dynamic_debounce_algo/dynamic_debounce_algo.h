// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>

typedef enum {
    DEBOUNCE_SYM_DEFER_G = 0,     // Global symmetric defer (default)
    DEBOUNCE_SYM_DEFER_PK,        // Per-key symmetric defer
    DEBOUNCE_SYM_DEFER_PR,        // Per-row symmetric defer
    DEBOUNCE_SYM_EAGER_PK,        // Per-key symmetric eager
    DEBOUNCE_SYM_EAGER_PR,        // Per-row symmetric eager
    DEBOUNCE_ASYM_EAGER_DEFER_PK, // Per-key asymmetric eager/defer
    DEBOUNCE_NONE,                // No debounce
    DEBOUNCE_ALGORITHM_COUNT
} debounce_algorithm_t;

// Set the current debounce algorithm at runtime
void debounce_set_algorithm(debounce_algorithm_t algorithm);

// Get the current debounce algorithm
debounce_algorithm_t debounce_get_algorithm(void);

// Get the name of a debounce algorithm
const char* debounce_get_algorithm_name(debounce_algorithm_t algorithm);

// Set the debounce time in milliseconds (max 255ms for most algorithms, 127ms for asym)
void debounce_set_time(uint8_t time_ms);

// Get the current debounce time in milliseconds
uint8_t debounce_get_time(void);

// Set debounce algorithm and time
void debounce_set_algorithm_and_time(debounce_algorithm_t algorithm, uint8_t time_ms);
