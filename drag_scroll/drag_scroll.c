// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#ifdef POINTING_DEVICE_ENABLE

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

static bool set_scrolling = false;

// Modify these values to adjust the scrolling speed
#    ifndef SCROLL_DIVISOR_H
#        define SCROLL_DIVISOR_H 8.0
#    endif // SCROLL_DIVISOR_H
#    ifndef SCROLL_DIVISOR_V
#        define SCROLL_DIVISOR_V 8.0
#    endif // SCROLL_DIVISOR_V

// Variables to store accumulated scroll values
static float scroll_accumulated_h = 0;
static float scroll_accumulated_v = 0;

static float scroll_divisor_h = 0;
static float scroll_divisor_v = 0;

// Function to handle mouse reports and perform drag scrolling
report_mouse_t pointing_device_task_drag_scroll(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (set_scrolling) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / scroll_divisor_h;
        scroll_accumulated_v += (float)mouse_report.y / scroll_divisor_h;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = (int8_t)scroll_accumulated_h;
        mouse_report.v = (int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return pointing_device_task_drag_scroll_kb(mouse_report);
}

// Function to handle key events and enable/disable drag scrolling
bool process_record_drag_scroll(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_drag_scroll_kb(keycode, record)) {
        return true;
    }

    switch (keycode) {
        case DRAG_SCROLL_TOGGLE:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            if (record->event.pressed) {
                set_scrolling = !set_scrolling;
            }
            break;
        case DRAG_SCROLL_MOMENTARY:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            set_scrolling = record->event.pressed;
            break;
        default:
            break;
    }
    return true;
}
#endif // POINTING_DEVICE_ENABLE

float get_drag_scroll_h_divisor(void) {
    return scroll_divisor_h;
}

float get_drag_scroll_v_divisor(void) {
    return scroll_divisor_v;
}

void set_drag_scroll_h_divisor(float divisor) {
    scroll_divisor_h = divisor;
}

void set_drag_scroll_v_divisor(float divisor) {
    scroll_divisor_v = divisor;
}

void set_drag_scroll_divisor(float divisor) {
    scroll_divisor_h = divisor;
    scroll_divisor_v = divisor;
}

bool get_drag_scroll_scrolling(void) {
    return set_scrolling;
}
void set_drag_scroll_scrolling(bool scrolling) {
    set_scrolling = scrolling;
}
