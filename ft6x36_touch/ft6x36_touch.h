// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>
#include <stdint.h>

// ──────────────────────────────────────────────────────────────────────────────
// I2C / hardware defaults (override in config.h)
// ──────────────────────────────────────────────────────────────────────────────

#ifndef FT6X36_I2C_ADDRESS
#    define FT6X36_I2C_ADDRESS 0x38 // 7-bit address
#endif

#ifndef FT6X36_I2C_TIMEOUT
#    define FT6X36_I2C_TIMEOUT 20
#endif

// Optional reset pin (active-low).  Define in config.h to enable hardware reset.
// Example: #define FT6X36_RESET_PIN B5
// If not defined the reset step is skipped.

#ifndef FT6X36_RESET_DURATION_MS
#    define FT6X36_RESET_DURATION_MS 5
#endif

#ifndef FT6X36_RESET_SETTLE_MS
#    define FT6X36_RESET_SETTLE_MS 300
#endif

// ──────────────────────────────────────────────────────────────────────────────
// Sensor register map
// ──────────────────────────────────────────────────────────────────────────────

#define FT6X36_REG_DEV_MODE      0x00
#define FT6X36_REG_GEST_ID       0x01
#define FT6X36_REG_TD_STATUS     0x02
#define FT6X36_REG_P1_XH         0x03
#define FT6X36_REG_P1_XL         0x04
#define FT6X36_REG_P1_YH         0x05
#define FT6X36_REG_P1_YL         0x06
#define FT6X36_REG_P1_WEIGHT     0x07
#define FT6X36_REG_P1_MISC       0x08
#define FT6X36_REG_P2_XH         0x09
#define FT6X36_REG_P2_XL         0x0A
#define FT6X36_REG_P2_YH         0x0B
#define FT6X36_REG_P2_YL         0x0C
#define FT6X36_REG_P2_WEIGHT     0x0D
#define FT6X36_REG_P2_MISC       0x0E
#define FT6X36_REG_TH_GROUP      0x80
#define FT6X36_REG_CTRL          0x86
#define FT6X36_REG_PERIOD_ACTIVE 0x88
#define FT6X36_REG_RADIAN_VALUE  0x91
#define FT6X36_REG_OFFSET_LR     0x92
#define FT6X36_REG_OFFSET_UD     0x93
#define FT6X36_REG_DISTANCE_LR   0x94
#define FT6X36_REG_DISTANCE_UD   0x95
#define FT6X36_REG_DISTANCE_ZOOM 0x96
#define FT6X36_REG_CHIP_ID       0xA3
#define FT6X36_REG_G_MODE        0xA4
#define FT6X36_REG_FIRMID        0xA6
#define FT6X36_REG_FOCALTECH_ID  0xA8 // expected: 0x11
#define FT6X36_REG_RELEASE_CODE  0xAF

// Touch-point event flags (bits 7:6 of P1_XH / P2_XH)
#define FT6X36_EVENT_PRESS_DOWN 0x00
#define FT6X36_EVENT_LIFT_UP    0x01
#define FT6X36_EVENT_CONTACT    0x02
#define FT6X36_EVENT_NONE       0x03

// Gesture IDs
#define FT6X36_GESTURE_NONE       0x00
#define FT6X36_GESTURE_MOVE_UP    0x10
#define FT6X36_GESTURE_MOVE_RIGHT 0x14
#define FT6X36_GESTURE_MOVE_DOWN  0x18
#define FT6X36_GESTURE_MOVE_LEFT  0x1C
#define FT6X36_GESTURE_ZOOM_IN    0x48
#define FT6X36_GESTURE_ZOOM_OUT   0x49

// ──────────────────────────────────────────────────────────────────────────────
// Tuning defaults (override in config.h)
// ──────────────────────────────────────────────────────────────────────────────

// Numerator/denominator for mouse-delta scaling (delta * NUM / DEN)
#ifndef FT6X36_TOUCH_SCALE_NUMERATOR
#    define FT6X36_TOUCH_SCALE_NUMERATOR 2
#endif
#ifndef FT6X36_TOUCH_SCALE_DENOMINATOR
#    define FT6X36_TOUCH_SCALE_DENOMINATOR 1
#endif

// Tap: finger-up within this many ms of finger-down → click
#ifndef FT6X36_TAP_TERM_MS
#    define FT6X36_TAP_TERM_MS 200
#endif

// Tap: total finger travel must be ≤ this (in raw sensor units) to count
#ifndef FT6X36_TAP_MAX_DISPLACEMENT
#    define FT6X36_TAP_MAX_DISPLACEMENT 30
#endif

// ──────────────────────────────────────────────────────────────────────────────
// Public data types
// ──────────────────────────────────────────────────────────────────────────────

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  event; // FT6X36_EVENT_*
    uint8_t  id;    // touch-point ID
    bool     valid;
} ft6x36_touch_point_t;

typedef struct {
    ft6x36_touch_point_t p[2];
    uint8_t              touch_count;
    uint8_t              gesture;
} ft6x36_touch_data_t;

// ──────────────────────────────────────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────────────────────────────────────

bool                ft6x36_init(void);
bool                ft6x36_is_connected(void);
ft6x36_touch_data_t ft6x36_read_touch(void);
void                ft6x36_reset(void);

// Runtime enable/disable
bool ft6x36_touch_get_enabled(void);
void ft6x36_touch_set_enabled(bool enable);
