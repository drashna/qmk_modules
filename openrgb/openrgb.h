// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

// Also, Copyright 2020 Kasper

#pragma once

// This is changed only when the command IDs change,
// so OpenRGB can detect compatible firmware.
#define OPENRGB_PROTOCOL_VERSION 0xD

#define RAW_EPSIZE 64

enum openrgb_command_id {
    OPENRGB_GET_PROTOCOL_VERSION = 1,
    OPENRGB_GET_QMK_VERSION,
    OPENRGB_GET_DEVICE_INFO,
    OPENRGB_GET_MODE_INFO,
    OPENRGB_GET_LED_INFO,
    OPENRGB_GET_ENABLED_MODES,

    OPENRGB_SET_MODE,
    OPENRGB_DIRECT_MODE_SET_SINGLE_LED,
    OPENRGB_DIRECT_MODE_SET_LEDS,
};

enum openrgb_responses {
    OPENRGB_FAILURE        = 25,
    OPENRGB_SUCCESS        = 50,
    OPENRGB_END_OF_MESSAGE = 100,
};

extern RGB g_openrgb_direct_mode_colors[RGB_MATRIX_LED_COUNT];

void openrgb_get_protocol_version(void);
void openrgb_get_qmk_version(void);
void openrgb_get_device_info(void);
void openrgb_get_mode_info(void);
void openrgb_get_led_info(uint8_t *data);
void openrgb_get_enabled_modes(void);

void openrgb_set_mode(uint8_t *data);
void openrgb_direct_mode_set_single_led(uint8_t *data);
void openrgb_direct_mode_set_leds(uint8_t *data);
