// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @brief Guarded Reset – wraps any keycode listed in
 *        guarded_reset_keycodes[] (default: QK_BOOTLOADER, QK_REBOOT,
 *        QK_CLEAR_EEPROM) so that it only fires after the key has been held
 *        for GUARDED_RESET_HOLD_MS milliseconds (default 3 s).
 *
 * To customise the list, define a strong guarded_reset_keycodes[] anywhere
 * in your build (keymap.c, <user>.c, etc.):
 *
 *   const uint16_t guarded_reset_keycodes[] = { QK_BOOT, MY_SAFE_KC };
 *
 * You may also override guarded_reset_keycode_count() or
 * guarded_reset_keycode_get() directly for fully dynamic behaviour.
 */

#include "guarded_reset.h"
#include "quantum.h"
#include "deferred_exec.h"
#include "process_keycode/process_quantum.h"
#define GUARDED_RESET_MAX_KEYCODES 3

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

/* -----------------------------------------------------------------------
 * Default guarded-keycode list.  Override by defining guarded_reset_keycodes[]
 * as a non-weak (strong) symbol anywhere in the firmware build.
 * ----------------------------------------------------------------------- */
__attribute__((weak)) const uint16_t guarded_reset_keycodes[] = {
    QK_BOOTLOADER,
    QK_REBOOT,
    QK_CLEAR_EEPROM,
};

__attribute__((weak)) uint16_t guarded_reset_keycode_count(void) {
    return ARRAY_SIZE(guarded_reset_keycodes);
}

__attribute__((weak)) uint16_t guarded_reset_keycode_get(uint16_t index) {
    if (index >= guarded_reset_keycode_count()) {
        return KC_NO;
    }
    return guarded_reset_keycodes[index];
}

static uint16_t            gr_hold_ms                            = GUARDED_RESET_HOLD_MS;
static deferred_executor_t gr_exec_table[GUARDED_RESET_MAX_KEYCODES] = {0};
static uint32_t            gr_exec_last_run                      = 0;
static deferred_token      gr_tokens[GUARDED_RESET_MAX_KEYCODES]   = {INVALID_DEFERRED_TOKEN};

uint16_t get_guarded_reset_hold_ms(void) {
    return gr_hold_ms;
}

void set_guarded_reset_hold_ms(uint16_t ms) {
    gr_hold_ms = (ms > 0) ? ms : GUARDED_RESET_HOLD_MS;
}

/**
 * @brief Deferred callback – looks up the keycode for the stored index and
 *        delegates to process_quantum so core changes are picked up
 *        automatically.  cb_arg is the list index cast to void*.
 */
static uint32_t gr_deferred_callback(uint32_t trigger_time, void *cb_arg) {
    uint16_t index = (uint16_t)(uintptr_t)cb_arg;
    if (index < GUARDED_RESET_MAX_KEYCODES) {
        gr_tokens[index] = INVALID_DEFERRED_TOKEN;
    }

    uint16_t keycode = guarded_reset_keycode_get(index);
    if (keycode != KC_NO) {
        keyrecord_t fake_record   = {0};
        fake_record.event.pressed = true;
        process_quantum(keycode, &fake_record);
    }

    return 0; /* do not repeat */
}

/**
 * @brief Intercept guarded keycodes before process_quantum sees them.
 *        On press we schedule the deferred callback for that keycode's own
 *        slot; on release we cancel only that slot.
 */
bool process_record_guarded_reset(uint16_t keycode, keyrecord_t *record) {
    uint16_t count = guarded_reset_keycode_count();
    for (uint16_t i = 0; i < count && i < GUARDED_RESET_MAX_KEYCODES; i++) {
        if (guarded_reset_keycode_get(i) != keycode) {
            continue;
        }

        if (record->event.pressed) {
            if (gr_tokens[i] != INVALID_DEFERRED_TOKEN) {
                cancel_deferred_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_tokens[i]);
            }
            gr_tokens[i] = defer_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_hold_ms, gr_deferred_callback, (void *)(uintptr_t)i);
        } else {
            if (gr_tokens[i] != INVALID_DEFERRED_TOKEN) {
                cancel_deferred_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_tokens[i]);
                gr_tokens[i] = INVALID_DEFERRED_TOKEN;
            }
        }

        /* Consume the keycode – process_quantum must not see it */
        return false;
    }

    return true;
}

/**
 * @brief Drive the private deferred executor table each scan cycle.
 */
void housekeeping_task_guarded_reset(void) {
    deferred_exec_advanced_task(gr_exec_table, ARRAY_SIZE(gr_exec_table), &gr_exec_last_run);
}
