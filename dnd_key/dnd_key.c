// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

bool process_record_dnd_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_DO_NOT_DISTURB:
             if (record->event.pressed) {
                host_system_send(0x9B);
            } else {
                host_system_send(0);
            }
            return false;
    }
    return true;
}
