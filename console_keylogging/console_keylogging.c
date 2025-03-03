// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(0, 1, 0);

static bool console_keylogger_enable = true;

bool process_record_console_keylogging(uint16_t keycode, keyrecord_t *record) {
    if (console_keylogger_enable) {
        xprintf("KL: 0x%04X, col: %2u, row: %2u, pressed: %1d, time: %5u, int: %1d, count: %u\n", keycode,
                record->event.key.col, record->event.key.row, record->event.pressed, record->event.time,
                record->tap.interrupted, record->tap.count);
    }

    return true;
}

bool console_keylogger_get_enabled(void) {
    return console_keylogger_enable;
}

void console_keylogger_set_enabled(bool enable) {
    console_keylogger_enable = enable;
}
