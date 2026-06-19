// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Smoothing factor for the exponential moving average.
 *
 * Range: 0.0 (maximum smoothing, very slow response) to
 *        1.0 (no smoothing, raw reports pass through).
 * Default: 0.4 — moderate smoothing that reduces jitter while keeping movement responsive.
 */
#ifndef POINTING_DEVICE_SMOOTHING_FACTOR
#    define POINTING_DEVICE_SMOOTHING_FACTOR 0.4f
#endif // POINTING_DEVICE_SMOOTHING_FACTOR

/**
 * @brief Milliseconds of inactivity after which the EMA state is reset.
 *
 * When the pointer has been still for this long, accumulated EMA values are
 * cleared so that the next movement starts from a clean state.
 * Default: 200 ms.
 */
#ifndef POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS
#    define POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS 200
#endif // POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS

/** Step applied per keypress when adjusting the smooth factor. Shift inverts, Ctrl multiplies by 10. */
#ifndef POINTING_DEVICE_SMOOTHING_FACTOR_STEP
#    define POINTING_DEVICE_SMOOTHING_FACTOR_STEP 0.05f
#endif // POINTING_DEVICE_SMOOTHING_FACTOR_STEP

/** Step (in ms) applied per keypress when adjusting the reset timeout. Shift inverts, Ctrl multiplies by 10. */
#ifndef POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP
#    define POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP 25
#endif // POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP

typedef struct {
    float    factor;
    uint32_t reset_timeout;
    bool     enabled;
} pointing_device_smoothing_config_t;

extern pointing_device_smoothing_config_t pointing_device_smoothing_config;

float    pointing_device_smoothing_get_factor(void);
void     pointing_device_smoothing_set_factor(float factor);
void     pointing_device_smoothing_factor_increment(void);
bool     pointing_device_smoothing_get_enabled(void);
void     pointing_device_smoothing_set_enabled(bool enable);
void     pointing_device_smoothing_toggle_enabled(void);
uint32_t pointing_device_smoothing_get_reset_timeout(void);
void     pointing_device_smoothing_set_reset_timeout(uint32_t timeout_ms);
void     pointing_device_smoothing_timeout_increment(void);

bool pointing_device_smoothing_get_mod_step_f(float step, float *out);
bool pointing_device_smoothing_get_mod_step_u32(uint32_t step, int32_t *out);

bool process_record_pointing_device_smoothing(uint16_t keycode, keyrecord_t *record);
void keyboard_post_init_pointing_device_smoothing(void);
