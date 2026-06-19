// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "console_keylogging.h"
#include "community_modules.h"
#include "eeconfig.h"
#include "print.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

typedef struct console_keylogging_config_t {
    bool enabled;
} console_keylogging_config_t;

static console_keylogging_config_t console_keylogging_config = {.enabled = false};

void eeconfig_read_console_keylogging(console_keylogging_config_t *value) {
    eeconfig_read_console_keylogging_datablock(value, 0, sizeof(console_keylogging_config_t));
}
void eeconfig_update_console_keylogging(console_keylogging_config_t *value) {
    eeconfig_update_console_keylogging_datablock(value, 0, sizeof(console_keylogging_config_t));
}

EECONFIG_DEBOUNCE_HELPER(console_keylogging, console_keylogging_config);

/**
 * @brief Print the details of a key event to the console. Allows for
 *
 * @param keycode The keycode of the key that was pressed or released.
 * @param record The keyrecord_t structure containing information about the key event.
 */
__attribute__((weak)) void console_keylogging_print_handler(uint16_t keycode, keyrecord_t *record) {
    xprintf("KL: 0x%04X, col: %2u, row: %2u, pressed: %1d, time: %5u, int: %1d, count: %u\n", keycode,
            record->event.key.col, record->event.key.row, record->event.pressed, record->event.time,
            record->tap.interrupted, record->tap.count);
}

/**
 * @brief Process a key event for the console keylogging feature.
 *
 * @param keycode The keycode of the key that was pressed or released.
 * @param record The keyrecord_t structure containing information about the key event.
 * @return true if the key event should be processed further, false otherwise.
 */
bool process_record_console_keylogging(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_console_keylogging_kb(keycode, record)) {
        return false;
    }
    if (console_keylogging_config.enabled) {
        console_keylogging_print_handler(keycode, record);
    }
    return true;
}

/**
 * @brief Perform housekeeping tasks for the console keylogging feature.
 */
void housekeeping_task_console_keylogging(void) {
    eeconfig_flush_console_keylogging_task(1000);
    housekeeping_task_console_keylogging_kb();
}

/**
 * @brief Initialize the console keylogging feature.
 */
void keyboard_post_init_console_keylogging(void) {
    if (!eeconfig_is_console_keylogging_datablock_valid()) {
        eeconfig_init_console_keylogging_datablock();
        console_keylogging_config.enabled = true;
        eeconfig_flush_console_keylogging(true);
    }

    eeconfig_init_console_keylogging();

    keyboard_post_init_console_keylogging_kb();
}

/**
 * @brief Get the enabled status of the console keylogging feature.
 *
 * @return true if enabled, false otherwise
 */
bool console_keylogging_get_enabled(void) {
    return console_keylogging_config.enabled;
}

/**
 * @brief Set the enabled status of the console keylogging feature.
 *
 * @param enable true to enable, false to disable
 */
void console_keylogging_set_enabled(bool enable) {
    console_keylogging_config.enabled = enable;
}
