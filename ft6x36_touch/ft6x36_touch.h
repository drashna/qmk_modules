// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

enum ft6x36_registers {
    FT6X36_REG_DEVICE_MODE         = 0x00,
    FT6X36_REG_GESTURE_ID          = 0x01,
    FT6X36_REG_NUM_TOUCHES         = 0x02,
    FT6X36_REG_P1_XH               = 0x03,
    FT6X36_REG_P1_XL               = 0x04,
    FT6X36_REG_P1_YH               = 0x05,
    FT6X36_REG_P1_YL               = 0x06,
    FT6X36_REG_P1_WEIGHT           = 0x07,
    FT6X36_REG_P1_MISC             = 0x08,
    FT6X36_REG_P2_XH               = 0x09,
    FT6X36_REG_P2_XL               = 0x0A,
    FT6X36_REG_P2_YH               = 0x0B,
    FT6X36_REG_P2_YL               = 0x0C,
    FT6X36_REG_P2_WEIGHT           = 0x0D,
    FT6X36_REG_P2_MISC             = 0x0E,
    FT6X36_REG_THRESHOLD           = 0x80,
    FT6X36_REG_FILTER_COEF         = 0x85,
    FT6X36_REG_CTRL                = 0x86,
    FT6X36_REG_TIME_ENTER_MONITOR  = 0x87,
    FT6X36_REG_TOUCHRATE_ACTIVE    = 0x88,
    FT6X36_REG_TOUCHRATE_MONITOR   = 0x89, // value in ms
    FT6X36_REG_RADIAN_VALUE        = 0x91,
    FT6X36_REG_OFFSET_LEFT_RIGHT   = 0x92,
    FT6X36_REG_OFFSET_UP_DOWN      = 0x93,
    FT6X36_REG_DISTANCE_LEFT_RIGHT = 0x94,
    FT6X36_REG_DISTANCE_UP_DOWN    = 0x95,
    FT6X36_REG_DISTANCE_ZOOM       = 0x96,
    FT6X36_REG_LIB_VERSION_H       = 0xA1,
    FT6X36_REG_LIB_VERSION_L       = 0xA2,
    FT6X36_REG_CHIPID              = 0xA3,
    FT6X36_REG_INTERRUPT_MODE      = 0xA4,
    FT6X36_REG_POWER_MODE          = 0xA5,
    FT6X36_REG_FIRMWARE_VERSION    = 0xA6,
    FT6X36_REG_PANEL_ID            = 0xA8,
    FT6X36_REG_STATE               = 0xBC,
};

enum ft6x36_power_mode {
    FT6X36_PMODE_ACTIVE    = 0x00,
    FT6X36_PMODE_MONITOR   = 0x01,
    FT6X36_PMODE_STANDBY   = 0x02,
    FT6X36_PMODE_HIBERNATE = 0x03,
};

#define FT6X36_VENDID 0x11
#define FT6206_CHIPID 0x06
#define FT6236_CHIPID 0x36
#define FT6336_CHIPID 0x64

#ifndef FT6X36_DEFAULT_THRESHOLD
#    define FT6X36_DEFAULT_THRESHOLD 22
#endif

enum ft6x36_raw_touch_event {
    PressDown,
    LiftUp,
    Contact,
    NoEvent,
};

enum ft6x36_touch_event {
    None,
    TouchStart,
    TouchMove,
    TouchEnd,
    Tap,
    DragStart,
    DragMove,
    DragEnd,
};

enum ft6x36_gesture_event {
    NoGesture  = 0x00,
    SwipeUp    = 0x10,
    SwipeRight = 0x14,
    SwipeDown  = 0x18,
    SwipeLeft  = 0x1C,
    ZoomIn     = 0x48,
    ZoomOut    = 0x49,
};

typedef struct {
    uint8_t device_mode;
    uint8_t gesture_id;
    uint8_t num_touches;
    union {
        struct {
            uint8_t msb_x    : 4;
            uint8_t reserved : 2;
            uint8_t event    : 2;
        } b;
        uint8_t w;
    } x_high;
    uint8_t x_low;
    union {
        struct {
            uint8_t msb_y    : 4;
            uint8_t touch_id : 4;
        } b;
        uint8_t w;
    } y_high;
    uint8_t y_low;
    uint8_t weight;
    uint8_t misc;
} ft6x36_data_t;
