// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboard_lock.h"
#include "quantum.h"

static host_driver_t *host_driver          = 0;
static bool           host_driver_disabled = false;

/**
 * @brief Mount/Umount the keyboard USB driver
 */
void set_keyboard_lock(bool status) {
    if (!status && !host_get_driver()) {
        host_set_driver(host_driver);
    } else if (status && host_get_driver()) {
        host_driver = host_get_driver();
        clear_keyboard();
        host_set_driver(0);
    } else if (status) {
        clear_keyboard();
    }

    host_driver_disabled = status;
}

/**
 * @brief Get the keyboard lock status
 *
 * @return true
 * @return false
 */
bool get_keyboard_lock(void) {
    return host_driver_disabled;
}

/**
 * @brief Toggles the keyboard lock status
 *
 */
 void toggle_keyboard_lock(void) {
    set_keyboard_lock(!host_driver_disabled);
}

bool process_record_keyboard_lock(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keyboard_lock_kb(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case CM_KEYBOARD_LOCK_TOGGLE:
                toggle_keyboard_lock();
                break;
        }
    }

    return true;
}
