// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device_smoothing.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

#if defined(__AVR__) || (defined(CORTEX_USE_FPU) && CORTEX_USE_FPU == FALSE)
#    pragma message "Warning: Pointing Device Smoothing module may not work properly without Floating Point support. Use at your own risk."
#endif

pointing_device_smoothing_config_t pointing_device_smoothing_config = {
    .factor        = POINTING_DEVICE_SMOOTHING_FACTOR,
    .reset_timeout = POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS,
    .enabled       = true,
};

// Exponential moving average state
static float ema_x = 0.0f;
static float ema_y = 0.0f;

// Rounding carry to reduce precision loss when converting float -> int8_t
static float carry_x = 0.0f;
static float carry_y = 0.0f;

static uint32_t smooth_timer = 0;

void eeconfig_read_pointing_device_smoothing(pointing_device_smoothing_config_t *value) {
    eeconfig_read_pointing_device_smoothing_datablock(value, 0, sizeof(pointing_device_smoothing_config_t));
}

void eeconfig_update_pointing_device_smoothing(pointing_device_smoothing_config_t *value) {
    eeconfig_update_pointing_device_smoothing_datablock(value, 0, sizeof(pointing_device_smoothing_config_t));
}

EECONFIG_DEBOUNCE_HELPER(pointing_device_smoothing, pointing_device_smoothing_config);

/**
 * @brief Returns the current EMA smoothing factor.
 * @return Factor in [0.0, 1.0].
 */
float pointing_device_smoothing_get_factor(void) {
    return pointing_device_smoothing_config.factor;
}

/**
 * @brief Sets the EMA smoothing factor.
 * @param factor New factor, clamped to [0.0, 1.0].
 *               1.0 = passthrough (no smoothing); 0.0 = maximum smoothing.
 */
void pointing_device_smoothing_set_factor(float factor) {
    if (factor >= 0.0f && factor <= 1.0f) {
        pointing_device_smoothing_config.factor = factor;
        eeconfig_flag_pointing_device_smoothing(true);
    }
}

/**
 * @brief Returns the current inactivity reset timeout in milliseconds.
 * @return Timeout in milliseconds.
 */
uint32_t pointing_device_smoothing_get_reset_timeout(void) {
    return pointing_device_smoothing_config.reset_timeout;
}

/**
 * @brief Sets the inactivity threshold after which the EMA state is reset.
 * @param timeout_ms Timeout in milliseconds. 0 disables the auto-reset.
 */
void pointing_device_smoothing_set_reset_timeout(uint32_t timeout_ms) {
    pointing_device_smoothing_config.reset_timeout = timeout_ms;
    eeconfig_flag_pointing_device_smoothing(true);
}

/**
 * @brief Returns true when EMA smoothing is active.
 * @return true if smoothing is enabled, false otherwise.
 */
bool pointing_device_smoothing_get_enabled(void) {
    return pointing_device_smoothing_config.enabled;
}

/**
 * @brief Enables or disables EMA smoothing.
 *
 * When disabled, the internal EMA state and rounding carry are cleared so
 * that stale history does not bleed into the next enabled session.
 *
 * @param enable true to enable smoothing; false to disable and reset state.
 */
void pointing_device_smoothing_set_enabled(bool enable) {
    pointing_device_smoothing_config.enabled = enable;
    eeconfig_flag_pointing_device_smoothing(true);
    if (!enable) {
        // Reset state when disabling so stale EMA doesn't bleed into next enable
        ema_x   = 0.0f;
        ema_y   = 0.0f;
        carry_x = 0.0f;
        carry_y = 0.0f;
    }
}

/** @brief Toggles EMA smoothing on/off. */
void pointing_device_smoothing_toggle_enabled(void) {
    pointing_device_smoothing_set_enabled(!pointing_device_smoothing_config.enabled);
}

/**
 * @brief Returns a modifier-adjusted float step for increment functions.
 * @param step     Base step value.
 * @param[out] out Adjusted step: negated if Shift is held, multiplied by 10 if Ctrl is held.
 * @return Always true.
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

/**
 * @brief Returns a modifier-adjusted integer step for increment functions.
 * @param step     Base step value (unsigned).
 * @param[out] out Adjusted signed step: negated if Shift is held, multiplied by 10 if Ctrl is held.
 * @return Always true.
 */
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

/**
 * @brief Increments the smoothing factor by one step, respecting held modifiers.
 *
 * Uses @ref POINTING_DEVICE_SMOOTHING_FACTOR_STEP as the base step.
 * Shift inverts the direction (decrement); Ctrl multiplies the step by 10.
 */
void pointing_device_smoothing_factor_increment(void) {
    float step = 0.0f;
    pointing_device_smoothing_get_mod_step_f(POINTING_DEVICE_SMOOTHING_FACTOR_STEP, &step);
    pointing_device_smoothing_set_factor(pointing_device_smoothing_config.factor + step);
}

/**
 * @brief Increments the reset timeout by one step, respecting held modifiers.
 *
 * Uses @ref POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP as the base step.
 * Shift inverts the direction (decrement); Ctrl multiplies the step by 10.
 * The result is clamped to a minimum of 0 ms.
 */
void pointing_device_smoothing_timeout_increment(void) {
    int32_t step = 0;
    pointing_device_smoothing_get_mod_step_u32(POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP, &step);
    // Prevent wrapping below zero
    int32_t new_timeout = (int32_t)pointing_device_smoothing_config.reset_timeout + step;
    if (new_timeout < 0) new_timeout = 0;
    pointing_device_smoothing_set_reset_timeout((uint32_t)new_timeout);
}

/**
 * @brief QMK pointing_device_task hook — applies EMA smoothing to X/Y axes.
 *
 * EMA formula: @f$ \text{ema} = \alpha \cdot \text{raw} + (1-\alpha) \cdot \text{ema} @f$
 *
 * A high alpha (near 1.0) passes raw values almost unchanged.
 * A low alpha (near 0.0) applies heavy smoothing at the cost of increased lag.
 *
 * Sub-pixel precision is preserved via a rounding carry that accumulates the
 * fractional part dropped when converting the float EMA back to int8_t.
 * The carry is reset when the pointer reverses direction or after the
 * inactivity timeout (@ref POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS).
 *
 * @param mouse_report Incoming mouse report.
 * @return Smoothed mouse report.
 */
report_mouse_t pointing_device_task_pointing_device_smoothing(report_mouse_t mouse_report) {
    if (!pointing_device_smoothing_config.enabled) {
        return pointing_device_task_pointing_device_smoothing_kb(mouse_report);
    }

    // Reset EMA after a period of inactivity
    if (mouse_report.x == 0 && mouse_report.y == 0) {
        if (timer_elapsed32(smooth_timer) > pointing_device_smoothing_config.reset_timeout) {
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
    ema_x = pointing_device_smoothing_config.factor * (float)mouse_report.x +
            (1.0f - pointing_device_smoothing_config.factor) * ema_x;
    ema_y = pointing_device_smoothing_config.factor * (float)mouse_report.y +
            (1.0f - pointing_device_smoothing_config.factor) * ema_y;

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

/**
 * @brief QMK process_record hook — handles smoothing keycodes.
 *
 * Responds to @c CM_SMOOTHING_TOGGLE, @c CM_SMOOTHING_FACTOR_INC, and
 * @c CM_SMOOTHING_TIMEOUT_INC on key-press events.
 *
 * @param keycode The keycode being processed.
 * @param record  Key event record.
 * @return true to continue normal QMK processing; false to stop.
 */
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

void housekeeping_task_pointing_device_smoothing(void) {
    eeconfig_flush_pointing_device_smoothing_task(1000);
    housekeeping_task_pointing_device_smoothing_kb();
}

void keyboard_post_init_pointing_device_smoothing(void) {
    eeconfig_init_pointing_device_smoothing();

    keyboard_post_init_pointing_device_smoothing_kb();
}

void eeconfig_init_pointing_device_smoothing_datablock(void) {
    pointing_device_smoothing_config = (pointing_device_smoothing_config_t){
        .factor        = POINTING_DEVICE_SMOOTHING_FACTOR,
        .reset_timeout = POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS,
        .enabled       = true,
    };
    eeconfig_flush_pointing_device_smoothing(true);
}
