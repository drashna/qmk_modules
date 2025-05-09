// Copyright 2024-2025 @tzarc
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device_gestures.h"
#include <math.h>

// TODO:
// Add support for ending gestures prematurely based on stopped movement

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#ifndef POINTING_DEVICE_GESTURES_TIMEOUT
#    define POINTING_DEVICE_GESTURES_TIMEOUT 1500
#endif // POINTING_DEVICE_GESTURES_TIMEOUT
#ifndef POINTING_DEVICE_GESTURES_THRESHOLD
#    define POINTING_DEVICE_GESTURES_THRESHOLD 500
#endif // POINTING_DEVICE_GESTURES_THRESHOLD

static bool    gesture_started  = false;
static bool    gesture_actioned = true;
static int32_t start_position_x = 0;
static int32_t start_position_y = 0;

static uint32_t            gesture_deferred_exec_check   = 0;
static deferred_executor_t gesture_exec[1]               = {0};
static deferred_token      gesture_token                 = INVALID_DEFERRED_TOKEN;
static uint16_t            gesture_deferred_exec_timeout = POINTING_DEVICE_GESTURES_TIMEOUT;
static uint16_t            gesture_movement_threshold    = POINTING_DEVICE_GESTURES_THRESHOLD;

bool pointing_device_gestures_is_started(void) {
    return gesture_started;
}

void pointing_device_gestures_start(void) {
    gesture_started  = true;
    gesture_actioned = false;
    start_position_x = 0;
    start_position_y = 0;
}

void pointing_device_gestures_end(void) {
    gesture_started = false;
    if (gesture_token != INVALID_DEFERRED_TOKEN) {
        if (cancel_deferred_exec_advanced(gesture_exec, 1, gesture_token)) {
            gesture_token = INVALID_DEFERRED_TOKEN;
        }
    }
}

void pointing_device_gestures_cancel(void) {
    pointing_device_gestures_end();
    gesture_actioned = true;
}

static uint32_t pointing_device_gestures_handler(uint32_t trigger_time, void *cb_arg) {
    if (gesture_started) {
        pointing_device_gestures_end();
    }
    return 0;
}

void pointing_device_gestures_timed_start(void) {
    if (gesture_token == INVALID_DEFERRED_TOKEN) {
        pointing_device_gestures_start();
        gesture_token =
            defer_exec_advanced(gesture_exec, 1, gesture_deferred_exec_timeout, pointing_device_gestures_handler, NULL);
    } else {
        pointing_device_gestures_cancel();
    }
}

__attribute__((weak)) void pointing_device_gestures_trigger(uint8_t direction) {
    tap_code16(pointing_device_gestures_get_keycode(direction));
}

void pointing_device_gestures_set_timeout(uint16_t timeout) {
    if (timeout != 0) {
        gesture_deferred_exec_timeout = timeout;
    } else {
        gesture_deferred_exec_timeout = POINTING_DEVICE_GESTURES_TIMEOUT;
    }
}

uint16_t pointing_device_gestures_get_timeout(void) {
    return gesture_deferred_exec_timeout;
}

void pointing_device_gestures_set_threshold(uint16_t threshold) {
    gesture_movement_threshold = threshold;
    pointing_device_gestures_cancel();
}

void housekeeping_task_pointing_device_accel(void) {
    deferred_exec_advanced_task(gesture_exec, 1, &gesture_deferred_exec_check);
}

report_mouse_t pointing_device_task_pointing_device_gestures(report_mouse_t mouse_report) {
    // One layer tap has kicked in, start accumulating mouse movement
    if (gesture_started) {
        start_position_x += mouse_report.x;
        start_position_y += mouse_report.y;

        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.v = 0;
        mouse_report.h = 0;
    }

    // Handle cardinal direction gestures
    if (!gesture_started && !gesture_actioned) {
        gesture_actioned = true;

        if (sqrtf(start_position_x * start_position_x + start_position_y * start_position_y) <
            gesture_movement_threshold) {
            return pointing_device_task_pointing_device_gestures_kb(mouse_report);
        }

        float   r  = atan2f(start_position_y, start_position_x);
        float   d  = 180 * r / M_PI;
        int16_t id = (int16_t)d;

        const int directional_split = 8;
        uint8_t   direction_idx =
            ((id + 360 + 360 / directional_split / 2) % 360 / (360 / directional_split)) % directional_split;

#if !defined(NO_PRINT) && defined(POINTING_DEVICE_DEBUG)
        const char *directions[] = {"E", "SE", "S", "SW", "W", "NW", "N", "NE"};
        xprintf("Mouse movement: %d, %d => %d degrees (%s, %u)\n", (int)start_position_x, (int)start_position_y, id,
                directions[direction_idx], direction_idx);
#endif // NO_PRINT
        pointing_device_gestures_trigger(direction_idx);
    }

    return pointing_device_task_pointing_device_gestures_kb(mouse_report);
}

bool process_record_pointing_device_gestures(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_pointing_device_gestures_kb(keycode, record)) {
        return true;
    }

    switch (keycode) {
        case CM_MOUSE_GESTURE_TAP:
            if (record->event.pressed) {
                pointing_device_gestures_timed_start();
            }
            break;
        case CM_MOUSE_GESTURE_HOLD:
            if (record->event.pressed) {
                pointing_device_gestures_start();
            } else {
                pointing_device_gestures_end();
            }
            break;
        default:
            break;
    }
    return true;
}
