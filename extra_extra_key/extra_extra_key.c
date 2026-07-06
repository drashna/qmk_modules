// Copyright 2025 Nick Brassel (@tzarc)
// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

bool process_record_extra_extra_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_DO_NOT_DISTURB:
             if (record->event.pressed) {
                host_system_send(0x9B);
            } else {
                host_system_send(0);
            }
            break;
        case KC_DICTATION:
             if (record->event.pressed) {
                host_consumer_send(0xC3);
            } else {
                host_consumer_send(0);
            }
            break;
    }
    return true;
}
