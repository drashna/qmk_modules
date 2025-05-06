// Copyright 2024 @pandrr
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#ifndef WIGGLE_BALL_TIMEOUT
#    define WIGGLE_BALL_TIMEOUT 250
#endif // WIGGLE_BALL_TIMEOUT
#ifndef WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT
#    define WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT 150
#endif // WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT

static bool     wiggle_ball_enabled                  = false;
static uint16_t wiggle_ball_timeout                  = WIGGLE_BALL_TIMEOUT;
static uint16_t wiggle_ball_direction_switch_timeout = WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT;

/**
 * @brief Gets the activated state of the wiggle ball.
 *
 * @return true has triggered
 * @return false hasn't triggered
 */
bool is_wiggle_ball_enabled(void) {
    return wiggle_ball_enabled;
}

/**
 * Sets the timeout value for the wiggle ball feature.
 *
 * If the provided timeout value is greater than 0, it will be used as the new
 * timeout. Otherwise, the default timeout value (WIGGLE_BALL_TIMEOUT) will be applied.
 *
 * @param timeout The desired timeout value in milliseconds. If 0 or less, the
 *                default timeout will be used.
 */
void set_wiggle_ball_timeout(uint16_t timeout) {
    if (timeout > 0) {
        wiggle_ball_timeout = timeout;
    } else {
        wiggle_ball_timeout = WIGGLE_BALL_TIMEOUT;
    }
}

/**
 * @brief Retrieves the current timeout value for the wiggle ball.
 *
 * This function returns the timeout value used for the wiggle ball feature.
 *
 * @return The timeout value as an unsigned 16-bit integer.
 */
uint16_t get_wiggle_ball_timeout(void) {
    return wiggle_ball_timeout;
}

/**
 * Sets the timeout value for the wiggle ball direction switch.
 *
 * If the provided timeout value is greater than 0, it will be used as the new
 * timeout. Otherwise, the default timeout value (WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT) will be applied.
 *
 * @param timeout The desired timeout value in milliseconds. If 0 or less, the
 *                default timeout will be used.
 */
void set_wiggle_ball_direction_switch_timeout(uint16_t timeout) {
    if (timeout > 0) {
        wiggle_ball_direction_switch_timeout = timeout;
    } else {
        wiggle_ball_direction_switch_timeout = WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT;
    }
}

/**
 * @brief Retrieves the timeout value for switching the direction of the wiggle ball.
 *
 * This function returns the current timeout value used to determine how long
 * the system waits before switching the direction of the wiggle ball.
 *
 * @return The timeout value for direction switching, in milliseconds.
 */
uint16_t get_wiggle_ball_direction_switch_timeout(void) {
    return wiggle_ball_direction_switch_timeout;
}

/**
 * @brief Calls the behavior for wiggle mouse ball state.  Such handing the drag scroll componont.
 *
 * This could be handled by the `pointing_device_task_wiggle_ball_kb()` function, but there isn't
 * a good, consistent place to place it, I think.
 *
 */
__attribute__((weak)) void wiggle_ball_state_change(bool enabled, report_mouse_t *report) {
#ifdef COMMUNITY_MODULE_DRAG_SCROLL_ENABLE
#    include "drag_scroll.h"
    // for drag scroll, we only need to do anything when the state changes. Drag scroll code handles everything else.
    if (enabled != get_drag_scroll_scrolling()) {
        set_drag_scroll_scrolling(enabled);
    }
#else
#    pragma message "Drag scroll is not enabled.  This works much better with the drag scroll module."
    if (enabled) {
        static mouse_xy_report_t scrollX = 0;
        static mouse_xy_report_t scrollY = 0;

        scrollX += report->x;
        scrollY += report->y;

        if (scrollX > 150) {
            report->h = 1;
            scrollX   = 0;
        }
        if (scrollX < -150) {
            report->h = -1;
            scrollX   = 0;
        }
        if (scrollY > 150) {
            report->v = -1;
            scrollY   = 0;
        }
        if (scrollY < -150) {
            report->v = 1;
            scrollY   = 0;
        }

        report->x = 0;
        report->y = 0;
    }
#endif
}

report_mouse_t pointing_device_task_wiggle_ball(report_mouse_t mouse_report) {
    static uint8_t  wiggle_ball_shake_count           = 0;
    static bool     last_direction                    = false;
    static uint16_t last_wiggle_direction_time        = 0;
    static uint16_t last_wiggle_direction_switch_time = 0;

    // wiggle ball detection
    if (timer_elapsed(last_wiggle_direction_time) > wiggle_ball_timeout) {
        if (timer_elapsed(last_wiggle_direction_switch_time) > wiggle_ball_direction_switch_timeout) {
            wiggle_ball_shake_count = 0;
        }

        if (mouse_report.x > 1 && mouse_report.y < 3 && !last_direction) {
            wiggle_ball_shake_count++;
            last_wiggle_direction_switch_time = timer_read();
            last_direction                    = !last_direction;
        }
        if (mouse_report.x < -1 && mouse_report.y < 3 && last_direction) {
            wiggle_ball_shake_count++;
            last_wiggle_direction_switch_time = timer_read();
            last_direction                    = !last_direction;
        }

        if (wiggle_ball_shake_count > 3) {
            wiggle_ball_enabled               = !wiggle_ball_enabled;
            wiggle_ball_shake_count           = 0;
            last_wiggle_direction_time        = timer_read();
            last_wiggle_direction_switch_time = timer_read();
        }

        wiggle_ball_state_change(wiggle_ball_enabled, &mouse_report);
    }

    return pointing_device_task_wiggle_ball_kb(mouse_report);
}
