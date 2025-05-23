// Copyright 2022 HorrorTroll <https://github.com/HorrorTroll>
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPD-License-Identifier: GPL-2.0-or-later

#include "bongocats.h"

__attribute__((weak)) const char PROGMEM bongocats_idle[BONGOCATS_IDLE_FRAMES][BONGOCATS_ANIM_SIZE] = {
    {// Idle 1 - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0,
     0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {// Idle 2 - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0,
     0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff,
     0xff, 0xff, 0x9f, 0x9f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {// Idle 3 - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0,
     0xc0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff,
     0xff, 0xff, 0x9f, 0x9f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0x7e, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1e, 0x1e, 0x3f, 0x3d, 0x3d, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0xf9, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {// Idle 4 - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0,
     0xc0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0x3c, 0x1c, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {// Idle 5 - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xe0,
     0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x0f, 0x06, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

#if BONGOCATS_PREP_FRAMES > 0
__attribute__((weak)) const char PROGMEM bongocats_prep[BONGOCATS_PREP_FRAMES][BONGOCATS_ANIM_SIZE] = {
    {// Prepare - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0,
     0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0xf0, 0xf8, 0xdc, 0x36, 0x3e, 0xee, 0xfc, 0xe0, 0x9c, 0xfe, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0x7e, 0xbe, 0xbc, 0xbc, 0x7c, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x44, 0xc4,
     0xc4, 0x68, 0x78, 0x59, 0x89, 0x91, 0x91, 0xd3, 0xf3, 0xa3, 0x23, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0x81, 0xfa, 0xff, 0xe6,
     0xe7, 0xfd, 0xff, 0xfc, 0xf3, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x13, 0x12, 0x3c, 0x2c, 0x26,
     0x27, 0x45, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x34, 0x3c, 0x2c, 0x44, 0xc8, 0xc8, 0xe9, 0xb9, 0x11, 0x11, 0x13, 0x93, 0xe3, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#endif // PREP_FRAMES

__attribute__((weak)) const char PROGMEM bongocats_tap[BONGOCATS_TAP_FRAMES][BONGOCATS_ANIM_SIZE] = {
    {// Tap left - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0,
     0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20,
     0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0xf0, 0xf8, 0xdc, 0x36, 0x3e, 0xee, 0xfc, 0xe0, 0x9c, 0xfe, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
     0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x30, 0x38, 0x2c, 0x44, 0xc4,
     0xc4, 0x68, 0x78, 0x59, 0x89, 0x91, 0x91, 0xd3, 0xf3, 0xa3, 0x23, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x3f, 0x1f, 0x07, 0x83, 0xc0, 0xe0,
     0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x13, 0x12, 0x3c, 0x2c, 0x26,
     0x27, 0x45, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x24, 0x18, 0x3c, 0x7e, 0x7f, 0x7f, 0x7f, 0xbf, 0x3f, 0x1f, 0x1f, 0x8f, 0xe7, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x11, 0x11, 0x21, 0x23, 0x23, 0x21, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {// Tap right - 128x32
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xce, 0x9c, 0xf8, 0xfc, 0xfe, 0x80, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xfc, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0,
     0xc0, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02,
     0x02, 0x38, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x31, 0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0xa0, 0xa0, 0x20, 0x20,
     0x40, 0x46, 0x4f, 0x5f, 0x9f, 0x9c, 0x90, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xcf, 0xcf, 0xff, 0xff, 0xbf, 0x7f, 0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0x7e, 0xbe, 0xbc, 0xbc, 0x7c, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f,
     0x0f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x8f, 0xc7, 0xe3, 0x31, 0x38, 0x2c, 0x04, 0x64,
     0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x8f, 0x27, 0x27, 0x27, 0xc7, 0xc7, 0x4f, 0x4f, 0x8f, 0x8f,
     0x9f, 0x9f, 0x1f, 0x1f, 0x3f, 0x3e, 0x3e, 0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0xfc, 0xfc, 0xff, 0x81, 0xfa, 0xff, 0xe6,
     0xe7, 0xfd, 0xff, 0xfc, 0xf3, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x18, 0x08, 0x08, 0x08, 0x18, 0x10,
     0x30, 0x60, 0x40, 0xc0, 0x86, 0x87, 0x85, 0xc4, 0x49, 0x69, 0x3e, 0x0e, 0x13, 0x11, 0x12, 0x12, 0x3d, 0x2d, 0x25,
     0x26, 0x44, 0x68, 0x78, 0x58, 0x9d, 0x97, 0x93, 0xe3, 0x62, 0x34, 0x3c, 0x2c, 0x26, 0xc7, 0xc5, 0x69, 0x39, 0x19,
     0x1d, 0x36, 0xa2, 0xe2, 0x62, 0x34, 0x3c, 0x2c, 0x44, 0xc8, 0xc8, 0xe9, 0xb9, 0x11, 0x11, 0x13, 0x93, 0xe3, 0x63,
     0x27, 0x27, 0x47, 0x47, 0xcf, 0xcf, 0x0f, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40,
     0x40, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};
