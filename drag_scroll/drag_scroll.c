// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "drag_scroll.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

static bool set_scrolling = false;

// Modify these values to adjust the scrolling speed
#ifndef SCROLL_DIVISOR_H
#    define SCROLL_DIVISOR_H 8
#endif // SCROLL_DIVISOR_H
#ifndef SCROLL_DIVISOR_V
#    define SCROLL_DIVISOR_V 8
#endif // SCROLL_DIVISOR_V

// Variables to store accumulated scroll values
static int8_t scroll_remainder_h = 0;
static int8_t scroll_remainder_v = 0;

static int8_t scroll_divisor_h = (int8_t)SCROLL_DIVISOR_H;
static int8_t scroll_divisor_v = (int8_t)SCROLL_DIVISOR_V;

/**
 * @brief Converts pointer movement into scroll wheel movement when enabled.
 *
 * When drag scrolling is active, X/Y pointer movement is translated into
 * horizontal/vertical wheel events and consumed from the outgoing report.
 * Fractional movement is preserved via remainders to keep scrolling smooth.
 *
 * @param mouse_report The current pointing device report.
 * @return report_mouse_t The transformed mouse report after drag scroll logic.
 */
report_mouse_t pointing_device_task_drag_scroll(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (set_scrolling) {
        mouse_report.h = (mouse_report.x + scroll_remainder_h) / scroll_divisor_h;
        mouse_report.v = (mouse_report.y + scroll_remainder_v) / scroll_divisor_v;

        // Update remainder scroll values through modulo
        scroll_remainder_h = (mouse_report.x + scroll_remainder_h) % scroll_divisor_h;
        scroll_remainder_v = (mouse_report.y + scroll_remainder_v) % scroll_divisor_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;

    } else {
        // Clear any left over remainders if not currently in drag scroll mode
        scroll_remainder_h = 0;
        scroll_remainder_v = 0;
    }
    return pointing_device_task_drag_scroll_kb(mouse_report);
}

/**
 * @brief Handles drag-scroll keycodes and updates scrolling state.
 *
 * Supported keycodes:
 * - DRAG_SCROLL_TOGGLE: toggles drag scrolling on press.
 * - DRAG_SCROLL_MOMENTARY: enables while held, disables on release.
 *
 * @param keycode The keycode being processed.
 * @param record The key event record for the keycode.
 * @return true Always returns true to allow normal key processing to continue.
 */
bool process_record_drag_scroll(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_drag_scroll_kb(keycode, record)) {
        return true;
    }

    switch (keycode) {
        case DRAG_SCROLL_TOGGLE:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            if (record->event.pressed) {
                set_drag_scroll_scrolling(!get_drag_scroll_scrolling());
            }
            break;
        case DRAG_SCROLL_MOMENTARY:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            set_drag_scroll_scrolling(record->event.pressed);
            break;
        default:
            break;
    }
    return true;
}

/**
 * @brief Gets the horizontal drag-scroll divisor.
 *
 * @return int8_t The horizontal divisor.
 */
int8_t get_drag_scroll_h_divisor(void) {
    return scroll_divisor_h;
}

/**
 * @brief Gets the vertical drag-scroll divisor.
 *
 * @return int8_t The vertical divisor.
 */
int8_t get_drag_scroll_v_divisor(void) {
    return scroll_divisor_v;
}

/**
 * @brief Sets the horizontal drag-scroll divisor.
 *
 * @param divisor The new horizontal divisor.
 */
void set_drag_scroll_h_divisor(int8_t divisor) {
    scroll_divisor_h = divisor;
}

/**
 * @brief Sets the vertical drag-scroll divisor.
 *
 * @param divisor The new vertical divisor.
 */
void set_drag_scroll_v_divisor(int8_t divisor) {
    scroll_divisor_v = divisor;
}

/**
 * @brief Sets both horizontal and vertical drag-scroll divisors.
 *
 * @param divisor The divisor to apply to both axes.
 */
void set_drag_scroll_divisor(int8_t divisor) {
    scroll_divisor_h = divisor;
    scroll_divisor_v = divisor;
}

/**
 * @brief Gets whether drag scrolling is currently enabled.
 *
 * @return true Drag scrolling is enabled.
 * @return false Drag scrolling is disabled.
 */
bool get_drag_scroll_scrolling(void) {
    return set_scrolling;
}

/**
 * @brief User override hook called when drag-scroll state changes.
 *
 * Override this in userspace to react to drag-scroll state transitions.
 *
 * @param scrolling The new drag-scroll enabled state.
 * @return bool Return value is currently ignored by the core implementation.
 */
__attribute__((weak)) bool set_drag_scroll_scrolling_user(bool scrolling) {
    return true;
}

/**
 * @brief Keyboard-level override hook for drag-scroll state changes.
 *
 * The default implementation forwards to set_drag_scroll_scrolling_user().
 *
 * @param scrolling The new drag-scroll enabled state.
 * @return bool Return value is currently ignored by the core implementation.
 */
__attribute__((weak)) bool set_drag_scroll_scrolling_kb(bool scrolling) {
    return set_drag_scroll_scrolling_user(scrolling);
}

/**
 * @brief Enables or disables drag scrolling.
 *
 * Updates the internal state and triggers keyboard/user callback hooks.
 *
 * @param scrolling true to enable drag scrolling, false to disable it.
 */
void set_drag_scroll_scrolling(bool scrolling) {
    set_scrolling = scrolling;
    set_drag_scroll_scrolling_kb(scrolling);
}
