// Copyright 2024-2025 @tzarc
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>

bool pointing_device_gestures_is_started(void);
void pointing_device_gestures_start(void);
void pointing_device_gestures_end(void);
void pointing_device_gestures_cancel(void);
void pointing_device_gestures_timed_start(void);

void     pointing_device_gestures_trigger(uint8_t direction);
uint16_t pointing_device_gestures_get_keycode(uint8_t direction);

void     pointing_device_gestures_set_timeout(uint16_t timeout);
uint16_t pointing_device_gestures_get_timeout(void);
void     pointing_device_gestures_set_threshold(uint16_t threshold);
uint16_t pointing_device_gestures_get_threshold(void);
