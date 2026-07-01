// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "velocikey.h"
#include "rgb_matrix.h"
#include "community_modules.h"
#include "eeconfig.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 0);

#ifndef TYPING_SPEED_MAX_VALUE
#    define TYPING_SPEED_MAX_VALUE 200
#endif

#ifndef TYPING_SPEED_MIN_VALUE
#    define TYPING_SPEED_MIN_VALUE 50
#endif

typedef struct velocikey_config_t {
    bool enabled;
    uint8_t min_speed;
    uint8_t max_speed;
} velocikey_config_t;

static velocikey_config_t velocikey_config;

STATIC_ASSERT(sizeof(velocikey_config_t) == EECONFIG_MODULE_VELOCIKEY_DATA_SIZE, "Velocikey config size mismatch");

static uint8_t typing_speed;

/**
 * @brief Read the Velocikey configuration from EEPROM.
 *
 * @param value Pointer to the velocikey_config_t structure to store the configuration.
 */
void eeconfig_read_velocikey(velocikey_config_t *value) {
    eeconfig_read_velocikey_datablock(value, 0, sizeof(velocikey_config_t));
}

/**
 * @brief Update the Velocikey configuration in EEPROM.
 *
 * @param value Pointer to the velocikey_config_t structure containing the new configuration.
 */
void eeconfig_update_velocikey(velocikey_config_t *value) {
    eeconfig_update_velocikey_datablock(value, 0, sizeof(velocikey_config_t));
}

EECONFIG_DEBOUNCE_HELPER(velocikey, velocikey_config);

/**
 * @brief Get the current state of Velocikey.
 *
 * @return true if Velocikey is enabled, false otherwise.
 */
bool velocikey_get_enabled(void) {
    return velocikey_config.enabled;
}

/**
 * @brief Enable Velocikey.
 */
void velocikey_enable(void) {
    velocikey_config.enabled = true;
    eeconfig_flag_velocikey(true);
}

/**
 * @brief Disable Velocikey.
 */
void velocikey_disable(void) {
    velocikey_config.enabled = false;
    typing_speed             = velocikey_config.min_speed;
    rgb_matrix_set_speed_noeeprom(typing_speed);
    eeconfig_flag_velocikey(true);
}

/**
 * @brief Toggle the state of Velocikey.
 */
void velocikey_toggle(void) {
    velocikey_config.enabled ? velocikey_disable() : velocikey_enable();
}

/**
 * @brief Set the minimum typing speed for Velocikey.
 *
 * @param min_speed The minimum typing speed.
 */
void velocikey_set_min_speed(uint8_t min_speed) {
    velocikey_config.min_speed = min_speed;
    eeconfig_flag_velocikey(true);
}

/**
 * @brief Get the minimum typing speed for Velocikey.
 *
 * @return The minimum typing speed.
 */
uint8_t velocikey_get_min_speed(void) {
    return velocikey_config.min_speed;
}

/**
 * @brief Set the maximum typing speed for Velocikey.
 *
 * @param max_speed The maximum typing speed.
 */
void velocikey_set_max_speed(uint8_t max_speed) {
    velocikey_config.max_speed = max_speed;
    eeconfig_flag_velocikey(true);
}

/**
 * @brief Get the maximum typing speed for Velocikey.
 *
 * @return The maximum typing speed.
 */
uint8_t velocikey_get_max_speed(void) {
    return velocikey_config.max_speed;
}

/**
 * @brief Accelerate the typing speed.
 */
void velocikey_accelerate(void) {
    if (typing_speed < velocikey_config.max_speed) {
        typing_speed += (velocikey_config.max_speed / 100);
    }
    if (rgb_matrix_get_speed() != typing_speed) {
        rgb_matrix_set_speed_noeeprom(typing_speed);
    }
}

/**
 * @brief Decelerate the typing speed.
 */
void velocikey_decelerate(void) {
    static uint16_t decay_timer = 0;

    if (timer_elapsed(decay_timer) > 500 || decay_timer == 0) {
        if (typing_speed > velocikey_config.min_speed) typing_speed -= 1;
        // Decay a little faster at half of max speed
        if (typing_speed > velocikey_config.max_speed / 2) typing_speed -= 1;
        // Decay even faster at 3/4 of max speed
        if (typing_speed > velocikey_config.max_speed / 4 * 3) typing_speed -= 2;
        decay_timer = timer_read();
    }
    if (rgb_matrix_get_speed() != typing_speed) {
        rgb_matrix_set_speed_noeeprom(typing_speed);
    }
}

/**
 * @brief Process a key record for Velocikey.
 *
 * @param keycode The keycode of the key event.
 * @param record The key event record.
 * @return true if the key event should be processed further, false otherwise.
 */
bool process_record_velocikey(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (velocikey_get_enabled()) {
            velocikey_accelerate();
        }

        if (keycode == QK_VELOCIKEY_TOGGLE) {
            velocikey_toggle();
        }
    }
    return process_record_velocikey_kb(keycode, record);
}

/**
 * @brief Housekeeping task for Velocikey.
 */
void housekeeping_task_velocikey(void) {
    if (velocikey_get_enabled()) {
        velocikey_decelerate();
    }
    eeconfig_flush_velocikey_task(1000);
    housekeeping_task_velocikey_kb();
}

/**
 * @brief Initialize Velocikey after keyboard initialization.
 */
void keyboard_post_init_velocikey(void) {
    eeconfig_init_velocikey();
    typing_speed = velocikey_config.min_speed;
    rgb_matrix_set_speed_noeeprom(velocikey_config.min_speed);
}
