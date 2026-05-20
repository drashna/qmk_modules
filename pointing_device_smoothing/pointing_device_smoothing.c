// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device_smoothing.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#if defined(__AVR__) || (defined(CORTEX_USE_FPU) && CORTEX_USE_FPU == FALSE)
#    pragma message "Warning: Pointing Device Smoothing module may not work properly without Floating Point support. Use at your own risk."
#endif

static bool     smooth_enabled       = true;
static float    smooth_factor        = POINTING_DEVICE_SMOOTHING_FACTOR;
static uint32_t smooth_reset_timeout = POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS;

// Exponential moving average state
static float ema_x = 0.0f;
static float ema_y = 0.0f;

// Rounding carry to reduce precision loss when converting float -> int8_t
static float carry_x = 0.0f;
static float carry_y = 0.0f;

static uint32_t smooth_timer = 0;

float pointing_device_smoothing_get_factor(void) {
    return smooth_factor;
}

void pointing_device_smoothing_set_factor(float factor) {
    if (factor >= 0.0f && factor <= 1.0f) {
        smooth_factor = factor;
    }
}

uint32_t pointing_device_smoothing_get_reset_timeout(void) {
    return smooth_reset_timeout;
}

void pointing_device_smoothing_set_reset_timeout(uint32_t timeout_ms) {
    smooth_reset_timeout = timeout_ms;
}

bool pointing_device_smoothing_get_enabled(void) {
    return smooth_enabled;
}

void pointing_device_smoothing_set_enabled(bool enable) {
    smooth_enabled = enable;
    if (!enable) {
        // Reset state when disabling so stale EMA doesn't bleed into next enable
        ema_x   = 0.0f;
        ema_y   = 0.0f;
        carry_x = 0.0f;
        carry_y = 0.0f;
    }
}

void pointing_device_smoothing_toggle_enabled(void) {
    pointing_device_smoothing_set_enabled(!smooth_enabled);
}

/**
 * Returns a step value adjusted by held modifiers.
 * Shift  → inverts the direction.
 * Ctrl   → multiplies by 10.
 */
bool pointing_device_smoothing_get_mod_step_f(float step, float *out) {
    const uint8_t mod_mask = get_mods();
    if (mod_mask & MOD_MASK_CTRL) {
        step *= 10.0f;
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        step *= -1.0f;
    }
    *out = step;
    return true;
}

bool pointing_device_smoothing_get_mod_step_u32(uint32_t step, int32_t *out) {
    const uint8_t mod_mask = get_mods();
    int32_t       s        = (int32_t)step;
    if (mod_mask & MOD_MASK_CTRL) {
        s *= 10;
    }
    if (mod_mask & MOD_MASK_SHIFT) {
        s *= -1;
    }
    *out = s;
    return true;
}

void pointing_device_smoothing_factor_increment(void) {
    float step = 0.0f;
    pointing_device_smoothing_get_mod_step_f(POINTING_DEVICE_SMOOTHING_FACTOR_STEP, &step);
    pointing_device_smoothing_set_factor(smooth_factor + step);
}

void pointing_device_smoothing_timeout_increment(void) {
    int32_t step = 0;
    pointing_device_smoothing_get_mod_step_u32(POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP, &step);
    // Prevent wrapping below zero
    int32_t new_timeout = (int32_t)smooth_reset_timeout + step;
    if (new_timeout < 0) new_timeout = 0;
    pointing_device_smoothing_set_reset_timeout((uint32_t)new_timeout);
}

/**
 * Core task hook — applies exponential moving average smoothing.
 *
 * EMA formula:  ema = alpha * raw + (1 - alpha) * ema
 *
 * A high alpha (near 1.0) passes raw values almost unchanged.
 * A low alpha (near 0.0) applies heavy smoothing / lag.
 */
report_mouse_t pointing_device_task_pointing_device_smoothing(report_mouse_t mouse_report) {
    if (!smooth_enabled) {
        return pointing_device_task_pointing_device_smoothing_kb(mouse_report);
    }

    // Reset EMA after a period of inactivity
    if (mouse_report.x == 0 && mouse_report.y == 0) {
        if (timer_elapsed32(smooth_timer) > smooth_reset_timeout) {
            ema_x   = 0.0f;
            ema_y   = 0.0f;
            carry_x = 0.0f;
            carry_y = 0.0f;
        }
        return pointing_device_task_pointing_device_smoothing_kb(mouse_report);
    }

    smooth_timer = timer_read32();

    // Reset carry when direction reverses to follow the user's hand
    if (mouse_report.x * carry_x < 0) carry_x = 0.0f;
    if (mouse_report.y * carry_y < 0) carry_y = 0.0f;

    // Apply EMA
    ema_x = smooth_factor * (float)mouse_report.x + (1.0f - smooth_factor) * ema_x;
    ema_y = smooth_factor * (float)mouse_report.y + (1.0f - smooth_factor) * ema_y;

    // Accumulate rounding carry to avoid dropping sub-pixel movement
    float out_x = ema_x + carry_x;
    float out_y = ema_y + carry_y;

    int8_t int_x = (int8_t)out_x;
    int8_t int_y = (int8_t)out_y;

    carry_x = out_x - (float)int_x;
    carry_y = out_y - (float)int_y;

    mouse_report.x = int_x;
    mouse_report.y = int_y;

    return pointing_device_task_pointing_device_smoothing_kb(mouse_report);
}

bool process_record_pointing_device_smoothing(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_pointing_device_smoothing_kb(keycode, record)) {
        return true;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case CM_SMOOTHING_TOGGLE:
                pointing_device_smoothing_toggle_enabled();
                break;
            case CM_SMOOTHING_FACTOR_INC:
                pointing_device_smoothing_factor_increment();
                break;
            case CM_SMOOTHING_TIMEOUT_INC:
                pointing_device_smoothing_timeout_increment();
                break;
        }
    }
    return true;
}

__attribute__((weak)) void keyboard_post_init_pointing_device_smoothing(void) {
    smooth_enabled       = true;
    smooth_factor        = POINTING_DEVICE_SMOOTHING_FACTOR;
    smooth_reset_timeout = POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS;

    keyboard_post_init_pointing_device_smoothing_kb();
}
