// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ft6x36_touch.h"
#include "pointing_device_internal.h"
#include "i2c_master.h"
#include "gpio.h"
#include "timer.h"
#include "debug.h"
#include "quantum.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

// ──────────────────────────────────────────────────────────────────────────────
// Internal state
// ──────────────────────────────────────────────────────────────────────────────

static bool ft6x36_connected = false;
static bool ft6x36_enabled   = true;
static bool ft6x36_pointing_enabled = true;

// Cursor tracking
static bool     touch_active = false;
static uint16_t last_x       = 0;
static uint16_t last_y       = 0;

// Tap detection (single finger)
static bool     tap_in_progress = false;
static uint32_t tap_start_ms    = 0;
static uint16_t tap_start_x     = 0;
static uint16_t tap_start_y     = 0;

// Two-finger tap detection
static bool     tap2_in_progress = false;
static uint32_t tap2_start_ms    = 0;

// Accumulated mouse buttons to send on the next report cycle
static uint8_t pending_buttons = 0;

// Last raw touch data captured in matrix_scan (used by both the absolute-position
// callbacks and the pointing-device processing path)
static ft6x36_touch_data_t ft6x36_last_touch_data = {0};

/**
 * @brief Clear internal gesture and tap state trackers.
 */
static void ft6x36_reset_touch_state(void) {
    touch_active     = false;
    tap_in_progress  = false;
    tap2_in_progress = false;
    pending_buttons  = 0;
}

// ──────────────────────────────────────────────────────────────────────────────
// I2C helpers
// ──────────────────────────────────────────────────────────────────────────────

/**
 * @brief Read one register byte from the FT6x36.
 *
 * @param reg Register address.
 * @param out Destination byte pointer.
 * @return true on successful I2C transaction.
 */
static inline bool ft6x36_read_reg(uint8_t reg, uint8_t *out) {
    return i2c_read_register(FT6X36_I2C_ADDRESS << 1, reg, out, 1, FT6X36_I2C_TIMEOUT) == I2C_STATUS_SUCCESS;
}

/**
 * @brief Read a contiguous register range from the FT6x36.
 *
 * @param reg Starting register address.
 * @param buf Destination buffer.
 * @param len Number of bytes to read.
 * @return true on successful I2C transaction.
 */
static inline bool ft6x36_read_regs(uint8_t reg, uint8_t *buf, uint16_t len) {
    return i2c_read_register(FT6X36_I2C_ADDRESS << 1, reg, buf, len, FT6X36_I2C_TIMEOUT) == I2C_STATUS_SUCCESS;
}

/**
 * @brief Write one register byte to the FT6x36.
 *
 * @param reg Register address.
 * @param val Value to write.
 * @return true on successful I2C transaction.
 */
static inline bool ft6x36_write_reg(uint8_t reg, uint8_t val) {
    return i2c_write_register(FT6X36_I2C_ADDRESS << 1, reg, &val, 1, FT6X36_I2C_TIMEOUT) == I2C_STATUS_SUCCESS;
}

// ──────────────────────────────────────────────────────────────────────────────
// Public: initialise
// ──────────────────────────────────────────────────────────────────────────────

/**
 * @brief Reset the FT6x36 via the configured reset pin.
 */
void ft6x36_reset(void) {
#ifdef FT6X36_RESET_PIN
    gpio_set_pin_output(FT6X36_RESET_PIN);
    gpio_write_pin_low(FT6X36_RESET_PIN);
    wait_ms(FT6X36_RESET_DURATION_MS);
    gpio_write_pin_high(FT6X36_RESET_PIN);
    wait_ms(FT6X36_RESET_SETTLE_MS);
#endif
}

/**
 * @brief Initialize I2C, probe the controller, and apply interrupt mode.
 *
 * @return true when the controller is detected.
 * @return false when probing fails.
 */
bool ft6x36_init(void) {
    ft6x36_reset();
    i2c_init();

    uint8_t id = 0;
    if (!ft6x36_read_reg(FT6X36_REG_FOCALTECH_ID, &id)) {
        pd_dprintf("FT6x36: device not found at 0x%02X\n", FT6X36_I2C_ADDRESS);
        ft6x36_connected = false;
        return false;
    }
    if (id != 0x11) {
        pd_dprintf("FT6x36: unexpected FOCALTECH_ID 0x%02X (expected 0x11)\n", id);
    }

    uint8_t chip_id = 0;
    ft6x36_read_reg(FT6X36_REG_CHIP_ID, &chip_id);
    pd_dprintf("FT6x36: chip_id=0x%02X focaltech_id=0x%02X\n", chip_id, id);

    // Configure interrupt pin as input with pull-up when defined
#ifdef FT6X36_INT_PIN
    gpio_set_pin_input_high(FT6X36_INT_PIN);
    pd_dprintf("FT6x36: INT pin configured\n");
#endif

    // Interrupt mode: trigger (0x00) when INT pin wired, polling (0x01) otherwise
#ifdef FT6X36_INT_PIN
    ft6x36_write_reg(FT6X36_REG_G_MODE, 0x00);
#else
    ft6x36_write_reg(FT6X36_REG_G_MODE, 0x01);
#endif

    ft6x36_connected = true;
    return true;
}

/**
 * @brief Get current FT6x36 connectivity state.
 *
 * @return true when device is connected.
 * @return false when device is not connected.
 */
bool ft6x36_is_connected(void) {
    return ft6x36_connected;
}

/**
 * @brief Get runtime touch-processing enable state.
 *
 * @return true when touch processing is enabled.
 * @return false when touch processing is disabled.
 */
bool ft6x36_touch_get_enabled(void) {
    return ft6x36_enabled;
}

/**
 * @brief Enable or disable touch processing at runtime.
 *
 * @param enable Set true to enable touch processing.
 */
void ft6x36_touch_set_enabled(bool enable) {
    ft6x36_enabled = enable;
    if (!enable) {
        ft6x36_reset_touch_state();
    }
}

/**
 * @brief Get runtime pointing report enable state.
 *
 * @return true when pointing report generation is enabled.
 * @return false when pointing report generation is disabled.
 */
bool ft6x36_pointing_get_enabled(void) {
    return ft6x36_pointing_enabled;
}

/**
 * @brief Enable or disable pointing report generation at runtime.
 *
 * @param enable Set true to enable pointing reports.
 */
void ft6x36_pointing_set_enabled(bool enable) {
    ft6x36_pointing_enabled = enable;
    if (!enable) {
        ft6x36_reset_touch_state();
    }
}

/**
 * @brief Return the most recent cached touch sample.
 *
 * @return Last touch snapshot.
 */
ft6x36_touch_data_t ft6x36_get_last_touch(void) {
    return ft6x36_last_touch_data;
}

/**
 * @brief Keyboard-level touch event hook.
 *
 * @param data Latest touch snapshot.
 */
__attribute__((weak)) void ft6x36_touch_event_kb(ft6x36_touch_data_t data) {
    ft6x36_touch_event_user(data);
}

/**
 * @brief User-level touch event hook.
 *
 * @param data Latest touch snapshot.
 */
__attribute__((weak)) void ft6x36_touch_event_user(ft6x36_touch_data_t data) {}

// ──────────────────────────────────────────────────────────────────────────────
// Public: read raw touch data
// ──────────────────────────────────────────────────────────────────────────────

/**
 * @brief Read, decode, and normalize FT6x36 touch data.
 *
 * @return Parsed touch snapshot.
 */
ft6x36_touch_data_t ft6x36_read_touch(void) {
    ft6x36_touch_data_t data = {0};

    if (!ft6x36_connected || !ft6x36_enabled) {
        pd_dprintf("FT6x36: not connected or enabled\n");
        return data;
    }

    // When an interrupt pin is configured, skip the I2C read if no event pending
#ifdef FT6X36_INT_PIN
    if (gpio_read_pin(FT6X36_INT_PIN)) {
        return data; // INT still high → no new data
    }
#endif

    // Read 15 bytes starting from GEST_ID (0x01) through P2_MISC (0x0E)
    uint8_t buf[14] = {0};
    if (!ft6x36_read_regs(FT6X36_REG_GEST_ID, buf, sizeof(buf))) {
        pd_dprintf("FT6x36: failed to read touch data\n");
        return data;
    }

    data.gesture     = buf[0];
    data.touch_count = buf[1] & 0x0F;
    if (data.touch_count > 2) data.touch_count = 2;

    pd_dprintf("FT6x36: tc=%u gest=0x%02X raw:", data.touch_count, data.gesture);
    for (uint8_t b = 0; b < 14; b++)
        pd_dprintf(" %02X", buf[b]);
    pd_dprintf("\n");

    for (uint8_t i = 0; i < 2; i++) {
        uint8_t base = 2 + i * 6; // P1 starts at buf[2], P2 at buf[8]
        uint8_t xh   = buf[base];
        uint8_t xl   = buf[base + 1];
        uint8_t yh   = buf[base + 2];
        uint8_t yl   = buf[base + 3];

        data.p[i].x     = ((uint16_t)(xh & 0x0F) << 8) | xl;
        data.p[i].y     = ((uint16_t)(yh & 0x0F) << 8) | yl;
        data.p[i].event = (xh >> 6) & 0x03;
        data.p[i].id    = (yh >> 4) & 0x0F;
        data.p[i].valid = (i < data.touch_count);

        if (data.p[i].valid) {
#ifdef FT6X36_TOUCH_ROTATE_180
            data.p[i].x = (data.p[i].x < FT6X36_TOUCH_MAX_X) ? (FT6X36_TOUCH_MAX_X - 1 - data.p[i].x) : 0;
            data.p[i].y = (data.p[i].y < FT6X36_TOUCH_MAX_Y) ? (FT6X36_TOUCH_MAX_Y - 1 - data.p[i].y) : 0;
#endif // FT6X36_TOUCH_ROTATE_180
            pd_dprintf("FT6x36: p%u x=%4u y=%4u ev=%u id=%u\n", i, data.p[i].x, data.p[i].y, data.p[i].event,
                       data.p[i].id);
        }
    }

    return data;
}

// ──────────────────────────────────────────────────────────────────────────────
// Internal: process touch data → mouse delta + button presses
// ──────────────────────────────────────────────────────────────────────────────

#if defined(POINTING_DEVICE_ENABLE)
#    include "pointing_device.h"

// Processes a pre-read touch snapshot into mouse deltas / tap clicks.
// Called from pointing_device_task using the data cached by matrix_scan.
/**
 * @brief Convert a touch snapshot into a pointing-device mouse report.
 *
 * @param mouse_report Incoming mouse report.
 * @param d Touch snapshot to process.
 * @return Updated mouse report.
 */
report_mouse_t ft6x36_process_touch(report_mouse_t mouse_report, ft6x36_touch_data_t d) {
    if (!ft6x36_connected || !ft6x36_enabled) {
        pd_dprintf("FT6x36: not connected or enabled\n");
        return mouse_report;
    }

    uint32_t now = timer_read32();

    if (d.touch_count == 1 && d.p[0].valid) {
        uint8_t ev = d.p[0].event;

        if (ev == FT6X36_EVENT_PRESS_DOWN) {
            pd_dprintf("FT6x36: DOWN x=%u y=%u\n", d.p[0].x, d.p[0].y);
            // Finger just placed; remember start position for tap detection
            touch_active    = true;
            last_x          = d.p[0].x;
            last_y          = d.p[0].y;
            tap_in_progress = true;
            tap_start_ms    = now;
            tap_start_x     = d.p[0].x;
            tap_start_y     = d.p[0].y;

        } else if ((ev == FT6X36_EVENT_CONTACT) && touch_active) {
            // Compute relative movement
            int32_t dx = (int32_t)d.p[0].x - last_x;
            int32_t dy = (int32_t)d.p[0].y - last_y;

            // Scale down
            dx = dx * FT6X36_TOUCH_SCALE_NUMERATOR / FT6X36_TOUCH_SCALE_DENOMINATOR;
            dy = dy * FT6X36_TOUCH_SCALE_NUMERATOR / FT6X36_TOUCH_SCALE_DENOMINATOR;

            mouse_report.x = (mouse_xy_report_t)CONSTRAIN_HID_XY(dx);
            mouse_report.y = (mouse_xy_report_t)CONSTRAIN_HID_XY(dy);
            pd_dprintf("FT6x36: MOVE dx=%d dy=%d -> mx=%d my=%d\n", (int)dx, (int)dy, (int)mouse_report.x,
                       (int)mouse_report.y);

            last_x = d.p[0].x;
            last_y = d.p[0].y;

            // Invalidate tap if moved too far
            if (tap_in_progress) {
                int32_t disp_x = (int32_t)d.p[0].x - tap_start_x;
                int32_t disp_y = (int32_t)d.p[0].y - tap_start_y;
                if (disp_x < 0) disp_x = -disp_x;
                if (disp_y < 0) disp_y = -disp_y;
                if (disp_x > FT6X36_TAP_MAX_DISPLACEMENT || disp_y > FT6X36_TAP_MAX_DISPLACEMENT) {
                    tap_in_progress = false;
                }
            }

        } else if (ev == FT6X36_EVENT_LIFT_UP) {
            // Check tap
            if (tap_in_progress && (timer_elapsed32(tap_start_ms) <= FT6X36_TAP_TERM_MS)) {
                pending_buttons |= MOUSE_BTN1;
            }
            touch_active    = false;
            tap_in_progress = false;
        }

    } else if (d.touch_count == 2 && d.p[0].valid && d.p[1].valid) {
        // Two-finger contact
        uint8_t ev0 = d.p[0].event;
        uint8_t ev1 = d.p[1].event;

        if (ev0 == FT6X36_EVENT_PRESS_DOWN || ev1 == FT6X36_EVENT_PRESS_DOWN) {
            tap2_in_progress = true;
            tap2_start_ms    = now;
            tap_in_progress  = false; // cancel single-tap tracking
            touch_active     = false;
        }

        if ((ev0 == FT6X36_EVENT_LIFT_UP || ev1 == FT6X36_EVENT_LIFT_UP) && tap2_in_progress) {
            if (timer_elapsed32(tap2_start_ms) <= FT6X36_TAP_TERM_MS) {
                pending_buttons |= MOUSE_BTN2;
            }
            tap2_in_progress = false;
        }

    } else {
        // No touch
        touch_active     = false;
        tap_in_progress  = false;
        tap2_in_progress = false;
    }

    // Inject any pending button presses (they'll be cleared next cycle)
    if (pending_buttons) {
        mouse_report.buttons |= pending_buttons;
        pending_buttons = 0;
    }

    return mouse_report;
}
#endif // POINTING_DEVICE_ENABLE

// ──────────────────────────────────────────────────────────────────────────────
// Community module hooks
// ──────────────────────────────────────────────────────────────────────────────

void keyboard_post_init_ft6x36_touch(void) {
    ft6x36_init();
    keyboard_post_init_ft6x36_touch_kb();
}

void housekeeping_task_ft6x36_touch(void) {
    if (ft6x36_connected && ft6x36_enabled) {
        ft6x36_last_touch_data = ft6x36_read_touch();
        ft6x36_touch_event_kb(ft6x36_last_touch_data);
    }
    housekeeping_task_ft6x36_touch_kb();
}

#if defined(POINTING_DEVICE_ENABLE)
void pointing_device_init_ft6x36_touch(void) {
    // Sensor already initialised in keyboard_post_init; nothing extra required.
    pointing_device_init_ft6x36_touch_kb();
}

report_mouse_t pointing_device_task_ft6x36_touch(report_mouse_t mouse_report) {
    if (ft6x36_pointing_enabled) {
        // Use the data already read by matrix_scan_ft6x36_touch() this cycle so
        // we never issue a second I2C read (important when an INT pin is wired).
        mouse_report = ft6x36_process_touch(mouse_report, ft6x36_last_touch_data);
    }
    return pointing_device_task_ft6x36_touch_kb(mouse_report);
}
#endif // POINTING_DEVICE_ENABLE
