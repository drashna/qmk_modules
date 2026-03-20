// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t twist;
    int16_t tilt_x;
    int16_t tilt_y;
} spacemouse_data_t;

bool              spacemouse_module_send_command(uint8_t cmd);
spacemouse_data_t spacemouse_module_get_data(void);
bool is_spacemouse_module_connected(void);
