// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include <lib/lib8tion/lib8tion.h>
#include "better_rgb_keys.h"

#ifndef BETTER_RGB_TICK_MS
#    define BETTER_RGB_TICK_MS 15
#endif // BETTER_RGB_TICK_MS

typedef struct {
    int8_t hue;
    int8_t sat;
    int8_t val;
    bool   is_rgb_matrix;
} rgb_change_state_t;

// timer to control color change speed
static uint16_t            change_tick  = BETTER_RGB_TICK_MS;
static rgb_change_state_t  change_state = {0};
static deferred_token      change_token = 0;

static uint32_t change_cb(uint32_t trigger_time, void *cb_arg) {
    if (change_state.hue != 0 || change_state.sat != 0 || change_state.val != 0) {
        HSV hsv = change_state.is_rgb_matrix ? rgb_matrix_get_hsv() : rgblight_get_hsv();
        hsv.h += change_state.hue;
        hsv.s = change_state.sat > 0 ? qadd8(hsv.s, (uint8_t)change_state.sat) : qsub8(hsv.s, (uint8_t)-change_state.sat);
        hsv.v = change_state.val > 0 ? qadd8(hsv.v, (uint8_t)change_state.val) : qsub8(hsv.v, (uint8_t)-change_state.val);
        if (change_state.is_rgb_matrix) {
            rgb_matrix_sethsv_noeeprom(hsv.h, hsv.s, hsv.v);
        } else {
            rgblight_sethsv_noeeprom(hsv.h, hsv.s, hsv.v);
        }
    }
    return change_tick;
}

uint16_t better_rgb_get_tick(void) {
    return change_tick;
}

void better_rgb_set_tick(uint16_t tick_ms) {
    if (tick_ms > 0) {
        change_tick = tick_ms;
    }
}

static void start_change(int8_t hue, int8_t sat, int8_t val, bool is_rgb_matrix) {
    change_state = (rgb_change_state_t){hue, sat, val, is_rgb_matrix};
    change_token = defer_exec(change_tick, change_cb, NULL);
}

bool process_record_better_rgb_keys(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case UG_HUEU: start_change( 1,  0,  0, false); return false;
            case UG_HUED: start_change(-1,  0,  0, false); return false;
            case UG_SATU: start_change( 0,  1,  0, false); return false;
            case UG_SATD: start_change( 0, -1,  0, false); return false;
            case UG_VALU: start_change( 0,  0,  1, false); return false;
            case UG_VALD: start_change( 0,  0, -1, false); return false;
            case RM_HUEU: start_change( 1,  0,  0, true);  return false;
            case RM_HUED: start_change(-1,  0,  0, true);  return false;
            case RM_SATU: start_change( 0,  1,  0, true);  return false;
            case RM_SATD: start_change( 0, -1,  0, true);  return false;
            case RM_VALU: start_change( 0,  0,  1, true);  return false;
            case RM_VALD: start_change( 0,  0, -1, true);  return false;
        }
    } else {
        switch (keycode) {
            case RM_HUEU ... RM_VALD:
            case UG_HUEU ... UG_VALD:
                if (change_token && cancel_deferred_exec(change_token)) {
                    change_token = 0;
                    if (change_state.is_rgb_matrix) {
                        eeconfig_force_flush_rgb_matrix();
                    } else {
                        eeconfig_update_rgblight_current();
                    }
                }
                return false;
        }
    }

    return true;
}
