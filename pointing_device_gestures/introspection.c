// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <platforms/progmem.h>

__attribute__((weak)) uint16_t pointing_device_gestures_get_keycode(uint8_t i) {
    if (i >= 8 || i < 0) {
        return 0;
    }

    return pgm_read_word(&pointing_device_gestures[i]);
}
