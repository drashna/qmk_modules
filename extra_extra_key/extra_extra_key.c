// Copyright 2025 Nick Brassel (@tzarc)
// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

bool process_record_extra_extra_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_DO_NOT_DISTURB:
            host_system_send(record->event.pressed ? 0x9B : 0);
            break;
        case KC_DICTATION:
            host_consumer_send(record->event.pressed ? 0xC3 : 0);
            break;
        case KC_GLOBE:
            host_consumer_send(record->event.pressed ? AC_NEXT_KEYBOARD_LAYOUT_SELECT : 0);
            break;
    }
    return true;
}
