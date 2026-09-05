// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "community_modules.h"
#include "eeconfig.h"
#include "tractyl.h"
#include <string.h>

#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif // SPLIT_KEYBOARD

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif // CONSOLE_ENABLE

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

#ifdef CHARYBDIS_MINIMUM_DEFAULT_DPI
#    define TRACTYL_MINIMUM_DEFAULT_DPI CHARYBDIS_MINIMUM_DEFAULT_DPI
#endif
#ifdef CHARYBDIS_DEFAULT_DPI_CONFIG_STEP
#    define TRACTYL_DEFAULT_DPI_CONFIG_STEP CHARYBDIS_DEFAULT_DPI_CONFIG_STEP
#endif
#ifdef CHARYBDIS_MINIMUM_SNIPING_DPI
#    define TRACTYL_MINIMUM_SNIPING_DPI CHARYBDIS_MINIMUM_SNIPING_DPI
#endif
#ifdef CHARYBDIS_SNIPING_DPI_CONFIG_STEP
#    define TRACTYL_SNIPING_DPI_CONFIG_STEP CHARYBDIS_SNIPING_DPI_CONFIG_STEP
#endif

#ifndef TRACTYL_MINIMUM_DEFAULT_DPI
#    define TRACTYL_MINIMUM_DEFAULT_DPI 400
#endif // TRACTYL_MINIMUM_DEFAULT_DPI

#ifndef TRACTYL_DEFAULT_DPI_CONFIG_STEP
#    define TRACTYL_DEFAULT_DPI_CONFIG_STEP 200
#endif // TRACTYL_DEFAULT_DPI_CONFIG_STEP

#ifndef TRACTYL_MINIMUM_SNIPING_DPI
#    define TRACTYL_MINIMUM_SNIPING_DPI 200
#endif // TRACTYL_MINIMUM_SNIPER_MODE_DPI

#ifndef TRACTYL_SNIPING_DPI_CONFIG_STEP
#    define TRACTYL_SNIPING_DPI_CONFIG_STEP 100
#endif // TRACTYL_SNIPING_DPI_CONFIG_STEP

typedef union {
    uint8_t raw;
    struct {
        uint8_t pointer_default_dpi : 4; // 16 steps available.
        uint8_t pointer_sniping_dpi : 2; // 4 steps available.
    };
} tractyl_config_t;

_Static_assert(sizeof(tractyl_config_t) <= EECONFIG_MODULE_TRACTYL_DATA_SIZE,
               "EECONFIG_MODULE_TRACTYL_DATA_SIZE is too small");

static tractyl_config_t tractyl_config = {0};

static bool is_sniping_enabled = false;

/**
 * \brief Set the value of `config` from EEPROM.
 *
 * Note that `is_sniping_enabled` is purposefully ignored since we do not want to persist this state to memory.  In
 * practice, this state is always written to maximize write-performances.  Therefore, we explicitly set it to `false` in
 * this function.
 */
static void read_tractyl_config_from_eeprom(tractyl_config_t *config) {
    eeconfig_read_tractyl_datablock(config, 0, sizeof(tractyl_config_t));
    is_sniping_enabled = false;
}

/**
 * \brief Save the value of `config` to eeprom.
 *
 * Note that all values are written verbatim, including whether sniper mode is
 * enabled.  `read_tractyl_config_from_eeprom(…)` resets this value to
 * `false` since it does not make sense to persist it across reboots of the
 * board.
 */
static void write_tractyl_config_to_eeprom(tractyl_config_t *config) {
    eeconfig_update_tractyl_datablock(config, 0, sizeof(tractyl_config_t));
}

/** \brief Return the current value of the pointer's default DPI. */
static uint16_t get_pointer_default_dpi(tractyl_config_t *config) {
    return (uint16_t)config->pointer_default_dpi * TRACTYL_DEFAULT_DPI_CONFIG_STEP + TRACTYL_MINIMUM_DEFAULT_DPI;
}

/** \brief Return the current value of the pointer's sniper-mode DPI. */
static uint16_t get_pointer_sniping_dpi(tractyl_config_t *config) {
    return (uint16_t)config->pointer_sniping_dpi * TRACTYL_SNIPING_DPI_CONFIG_STEP + TRACTYL_MINIMUM_SNIPING_DPI;
}

/** \brief Set the appropriate DPI for the input config. */
static void maybe_update_pointing_device_cpi(tractyl_config_t *config) {
    if (is_sniping_enabled) {
        pointing_device_set_cpi(get_pointer_sniping_dpi(config));
    } else {
        pointing_device_set_cpi(get_pointer_default_dpi(config));
    }
}

/**
 * \brief Update the pointer's default DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to TRACTYL_DEFAULT_DPI_CONFIG_STEP.
 */
static void step_pointer_default_dpi(tractyl_config_t *config, bool forward) {
    config->pointer_default_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

/**
 * \brief Update the pointer's sniper-mode DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to TRACTYL_SNIPING_DPI_CONFIG_STEP.
 */
static void step_pointer_sniping_dpi(tractyl_config_t *config, bool forward) {
    config->pointer_sniping_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

uint16_t tractyl_get_pointer_default_dpi(void) {
    return get_pointer_default_dpi(&tractyl_config);
}

uint16_t tractyl_get_pointer_sniping_dpi(void) {
    return get_pointer_sniping_dpi(&tractyl_config);
}

void tractyl_cycle_pointer_default_dpi_noeeprom(bool forward) {
    step_pointer_default_dpi(&tractyl_config, forward);
}

void tractyl_cycle_pointer_default_dpi(bool forward) {
    step_pointer_default_dpi(&tractyl_config, forward);
    write_tractyl_config_to_eeprom(&tractyl_config);
}

void tractyl_cycle_pointer_sniping_dpi_noeeprom(bool forward) {
    step_pointer_sniping_dpi(&tractyl_config, forward);
}

void tractyl_cycle_pointer_sniping_dpi(bool forward) {
    step_pointer_sniping_dpi(&tractyl_config, forward);
    write_tractyl_config_to_eeprom(&tractyl_config);
}

bool tractyl_get_pointer_sniping_enabled(void) {
    return is_sniping_enabled;
}

void tractyl_set_pointer_sniping_enabled(bool enable) {
    is_sniping_enabled = enable;
    maybe_update_pointing_device_cpi(&tractyl_config);
}

/**
 * \brief Outputs the Charybdis configuration to console.
 *
 * Prints the in-memory configuration structure to console, for debugging.
 * Includes:
 *   - raw value
 *   - drag-scroll: on/off
 *   - sniping: on/off
 *   - default DPI: internal table index/actual DPI
 *   - sniping DPI: internal table index/actual DPI
 */
void debug_tractyl_config_to_console(tractyl_config_t *config) {
    dprintf("(tractyl) process_record_tractyld: config = {\n"
            "\traw = 0x%X,\n"
            "\t{\n"
            "\t\tis_sniping_enabled=%u\n"
            "\t\tdefault_dpi=0x%X (%u)\n"
            "\t\tsniping_dpi=0x%X (%u)\n"
            "\t}\n"
            "}\n",
            config->raw, is_sniping_enabled, config->pointer_default_dpi, get_pointer_default_dpi(config),
            config->pointer_sniping_dpi, get_pointer_sniping_dpi(config));
}

/** \brief Whether SHIFT mod is enabled. */
static bool has_shift_mod(void) {
#ifdef NO_ACTION_ONESHOT
    return mod_config(get_mods()) & MOD_MASK_SHIFT;
#else
    return mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
#endif // NO_ACTION_ONESHOT
}

bool process_record_tractyl(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_tractyl_kb(keycode, record)) {
        return false;
    }
    switch (keycode) {
        case POINTER_DEFAULT_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                tractyl_cycle_pointer_default_dpi(/* forward= */ !has_shift_mod());
                debug_tractyl_config_to_console(&tractyl_config);
            }
            break;
        case POINTER_DEFAULT_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                tractyl_cycle_pointer_default_dpi(/* forward= */ has_shift_mod());
                debug_tractyl_config_to_console(&tractyl_config);
            }
            break;
        case POINTER_SNIPING_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                tractyl_cycle_pointer_sniping_dpi(/* forward= */ !has_shift_mod());
                debug_tractyl_config_to_console(&tractyl_config);
            }
            break;
        case POINTER_SNIPING_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                tractyl_cycle_pointer_sniping_dpi(/* forward= */ has_shift_mod());
                debug_tractyl_config_to_console(&tractyl_config);
            }
            break;
        case SNIPING_MODE:
            tractyl_set_pointer_sniping_enabled(record->event.pressed);
            debug_tractyl_config_to_console(&tractyl_config);
            break;
        case SNIPING_MODE_TOGGLE:
            if (record->event.pressed) {
                tractyl_set_pointer_sniping_enabled(!tractyl_get_pointer_sniping_enabled());
                debug_tractyl_config_to_console(&tractyl_config);
            }
            break;
    }

    return true;
}

void keyboard_pre_init_tractyl(void) {
    read_tractyl_config_from_eeprom(&tractyl_config);
    keyboard_pre_init_tractyl_kb();
}

void keyboard_post_init_tractyl(void) {
    maybe_update_pointing_device_cpi(&tractyl_config);
#ifdef SPLIT_KEYBOARD
    void tractyl_config_sync_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                     uint8_t target2initiator_buffer_size, void *target2initiator_buffer);
    transaction_register_rpc(RPC_ID_TRACTYL_CONFIG_SYNC, tractyl_config_sync_handler);
#endif // SPLIT_KEYBOARD
    keyboard_post_init_tractyl_kb();
}

#ifdef SPLIT_KEYBOARD
typedef struct PACKED {
    tractyl_config_t config;
    bool             is_sniping_enabled;
} tractyl_runtime_state_t;

void tractyl_config_sync_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                 uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(tractyl_runtime_state_t)) {
        tractyl_runtime_state_t state;
        memcpy(&state, initiator2target_buffer, sizeof(tractyl_runtime_state_t));
        tractyl_config = state.config;
        if (is_sniping_enabled != state.is_sniping_enabled) {
            is_sniping_enabled = state.is_sniping_enabled;
            maybe_update_pointing_device_cpi(&tractyl_config);
        }
    }
}
void housekeeping_task_tractyl(void) {
    if (is_keyboard_master()) {
        // Keep track of the last state, so that we can tell if we need to propagate to slave
        static tractyl_runtime_state_t last_tractyl_config    = {0};
        tractyl_runtime_state_t        current_tractyl_config = {
            .config             = tractyl_config,
            .is_sniping_enabled = is_sniping_enabled,
        };
        static uint32_t last_sync  = 0;
        bool            needs_sync = false;

        // Check if the state values are different
        if (memcmp(&current_tractyl_config, &last_tractyl_config, sizeof(tractyl_runtime_state_t))) {
            needs_sync = true;
            memcpy(&last_tractyl_config, &current_tractyl_config, sizeof(tractyl_runtime_state_t));
        }
        // Send to slave every 500ms regardless of state change
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested
        if (needs_sync) {
            if (transaction_rpc_send(RPC_ID_TRACTYL_CONFIG_SYNC, sizeof(tractyl_runtime_state_t),
                                     &current_tractyl_config)) {
                last_sync = timer_read32();
            }
        }
    }
    housekeeping_task_tractyl_kb();
}
#endif // SPLIT_KEYBOARD

#if defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
__attribute__((weak)) bool is_mouse_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KB_KEYCODE_RANGE:
        case POINTER_DEFAULT_DPI_FORWARD:
        case POINTER_DEFAULT_DPI_REVERSE:
        case POINTER_SNIPING_DPI_FORWARD:
        case POINTER_SNIPING_DPI_REVERSE:
        case SNIPING_MODE:
        case SNIPING_MODE_TOGGLE:
            return true;
        default:
            return is_mouse_record_user(keycode, record);
    }
}
#endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE
