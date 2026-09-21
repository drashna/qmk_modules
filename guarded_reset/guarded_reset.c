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
#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif // SPLIT_KEYBOARD

#define GUARDED_RESET_MAX_KEYCODES 3

#ifndef GUARDED_RESET_HOLD_MS
#    define GUARDED_RESET_HOLD_MS 3000
#endif
#ifndef GUARDED_RESET_HOLD_MS_STEP
#    define GUARDED_RESET_HOLD_MS_STEP 100
#endif
#ifndef FORCED_SYNC_THROTTLE_MS
#    define FORCED_SYNC_THROTTLE_MS 100
#endif // FORCED_SYNC_THROTTLE_MS

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

/* -----------------------------------------------------------------------
 * Default guarded-keycode list.  Override by defining guarded_reset_keycodes[]
 * as a non-weak (strong) symbol anywhere in the firmware build.
 * ----------------------------------------------------------------------- */

__attribute__((weak)) const guarded_reset_entry_t guarded_reset_keycodes[] = {
    {QK_BOOTLOADER, process_quantum},
    {QK_REBOOT, process_quantum},
    {QK_CLEAR_EEPROM, process_quantum},
};

__attribute__((weak)) uint16_t guarded_reset_keycode_count(void) {
    return ARRAY_SIZE(guarded_reset_keycodes);
}

__attribute__((weak)) uint16_t guarded_reset_keycode_get(uint16_t index) {
    if (index >= guarded_reset_keycode_count()) {
        return KC_NO;
    }
    return guarded_reset_keycodes[index].keycode;
}

static deferred_executor_t gr_exec_table[GUARDED_RESET_MAX_KEYCODES] = {0};
static uint32_t            gr_exec_last_run                          = 0;
static deferred_token      gr_tokens[GUARDED_RESET_MAX_KEYCODES]     = {INVALID_DEFERRED_TOKEN};

typedef struct PACKED {
    uint16_t hold_ms;
} guarded_reset_config_t;

static guarded_reset_config_t gr_config = {
    .hold_ms = GUARDED_RESET_HOLD_MS,
};

STATIC_ASSERT(EECONFIG_MODULE_GUARDED_RESET_DATA_SIZE <= sizeof(uint16_t),
              "EECONFIG_MODULE_GUARDED_RESET_DATA_SIZE must fit within a uint16_t.");

// Helpers required to bind to debounce helper
void eeconfig_read_guarded_reset(guarded_reset_config_t *value) {
    eeconfig_read_guarded_reset_datablock(value, 0, sizeof(guarded_reset_config_t));
}
void eeconfig_update_guarded_reset(guarded_reset_config_t *value) {
    eeconfig_update_guarded_reset_datablock(value, 0, sizeof(guarded_reset_config_t));
}

EECONFIG_DEBOUNCE_HELPER(guarded_reset, gr_config);

/**
 * @brief Initialize the guarded reset configuration datablcok in EEPROM.
 *
 * This function sets the default hold time for the guarded reset feature
 * in the EEPROM, ensuring that the configuration is available on startup.
 */
void eeconfig_init_guarded_reset_datablock(void) {
    guarded_reset_config_t default_config = {
        .hold_ms = GUARDED_RESET_HOLD_MS,
    };
    eeconfig_update_guarded_reset(&default_config);
}

/**
 * @brief Get the guarded reset hold ms object
 *
 * @return uint16_t
 */
uint16_t get_guarded_reset_hold_ms(void) {
    return gr_config.hold_ms;
}

/**
 * @brief Set the hold time for the guarded reset feature.
 *
 * @param ms The desired hold time in milliseconds. If 0, the default hold time is used.
 */
void set_guarded_reset_hold_ms(uint16_t ms) {
    gr_config.hold_ms = (ms > 0) ? ms : GUARDED_RESET_HOLD_MS;
    eeconfig_flag_guarded_reset(true);
}

/**
 * @brief Adjust the hold time for the guarded reset feature in steps.
 *
 * @param increase If true, increase the hold time; if false, decrease it.
 */
void guarded_reset_hold_ms_step(bool increase) {
    gr_config.hold_ms += (increase) ? GUARDED_RESET_HOLD_MS_STEP : -GUARDED_RESET_HOLD_MS_STEP;
    eeconfig_flag_guarded_reset(true);
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

    if (index < guarded_reset_keycode_count()) {
        uint16_t                keycode = guarded_reset_keycodes[index].keycode;
        guarded_reset_handler_t handler = guarded_reset_keycodes[index].handler;
        if (keycode != KC_NO && handler != NULL) {
            keyrecord_t fake_record   = {0};
            fake_record.event.pressed = true;
            handler(keycode, &fake_record);
        }
    }

    return 0; /* do not repeat */
}

/**
 * @brief Intercept guarded keycodes before process_quantum sees them.
 *        On press we schedule the deferred callback for that keycode's own
 *        slot; on release we cancel only that slot.
 */
bool pre_process_record_guarded_reset(uint16_t keycode, keyrecord_t *record) {
    uint16_t count = guarded_reset_keycode_count();
    for (uint16_t i = 0; i < count && i < GUARDED_RESET_MAX_KEYCODES; i++) {
        if (guarded_reset_keycode_get(i) != keycode || gr_config.hold_ms == 0) {
            continue;
        }

        if (record->event.pressed) {
            if (gr_tokens[i] != INVALID_DEFERRED_TOKEN) {
                cancel_deferred_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_tokens[i]);
            }
            gr_tokens[i] = defer_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_config.hold_ms,
                                               gr_deferred_callback, (void *)(uintptr_t)i);
        } else {
            if (gr_tokens[i] != INVALID_DEFERRED_TOKEN) {
                cancel_deferred_exec_advanced(gr_exec_table, ARRAY_SIZE(gr_exec_table), gr_tokens[i]);
                gr_tokens[i] = INVALID_DEFERRED_TOKEN;
            }
        }

        /* Consume the keycode – process_quantum must not see it */
        return false && pre_process_record_guarded_reset_kb(keycode, record);
    }

    return pre_process_record_guarded_reset_kb(keycode, record);
}

/**
 * @brief Intercept guarded keycodes before process_record sees them.
 * @param keycode The keycode being processed.
 * @param record  The key event record.
 * @return true if the keycode should be further processed, false if it should be consumed.
 */

bool process_record_guarded_reset(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_guarded_reset_kb(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case CM_GUARDED_RESET_INC:
            guarded_reset_hold_ms_step(true);
            return false;
        case CM_GUARDED_RESET_DEC:
            guarded_reset_hold_ms_step(false);
            return false;
    }
    return true;
}

#ifdef SPLIT_KEYBOARD
/**
 * @brief Handle synchronization of guarded reset configuration from the master to the slave.
 *
 * @param initiator2target_buffer_size Size of the data sent from the master to the slave.
 * @param initiator2target_buffer     Pointer to the data sent from the master to the slave.
 * @param target2initiator_buffer_size Size of the data sent from the slave to the master.
 * @param target2initiator_buffer     Pointer to the data sent from the slave to the master.
 */
static void module_sync_slave_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                      uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    const guarded_reset_config_t *sync_data = (const guarded_reset_config_t *)initiator2target_buffer;
    if (sync_data->hold_ms != gr_config.hold_ms) {
        set_guarded_reset_hold_ms(sync_data->hold_ms);
    }
}
#endif // SPLIT_KEYBOARD
/**
 * @brief Initialize the guarded reset configuration after keyboard post-init.
 */
void keyboard_post_init_guarded_reset(void) {
    /* Initialize the guarded reset configuration datablcok in EEPROM */
    eeconfig_init_guarded_reset();
#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_GUARDED_RESET_SYNC, module_sync_slave_handler);
#endif // SPLIT_KEYBOARD
}

/**
 * @brief Drive the private deferred executor table each scan cycle.
 */
void housekeeping_task_guarded_reset(void) {
    eeconfig_flush_guarded_reset(false);
#ifdef SPLIT_KEYBOARD
    if (is_keyboard_master()) {
        bool                          need_sync        = false;
        static fast_timer_t           last_sync        = 0;
        static guarded_reset_config_t last_sync_config = {0};
        if (memcmp(&last_sync_config, &gr_config, sizeof(guarded_reset_config_t)) != 0 ||
            timer_elapsed_fast(last_sync) > FORCED_SYNC_THROTTLE_MS) {
            need_sync = true;
        }
        if (need_sync && transaction_rpc_send(RPC_ID_GUARDED_RESET_SYNC, sizeof(gr_config), &gr_config)) {
            last_sync        = timer_read_fast();
            last_sync_config = gr_config;
        }
    }
#endif // SPLIT_KEYBOARD

    deferred_exec_advanced_task(gr_exec_table, ARRAY_SIZE(gr_exec_table), &gr_exec_last_run);
}
