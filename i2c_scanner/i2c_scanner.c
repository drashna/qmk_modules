// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "i2c_master.h"
#include "community_modules.h"
#include "eeconfig.h"
#include "debug.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 3);

#ifndef I2C_SCANNER_TIMEOUT
#    define I2C_SCANNER_TIMEOUT 50
#endif // I2C_SCANNER_TIMEOUT

typedef struct i2c_scanner_config_t {
    bool enabled;
} i2c_scanner_config_t;

static i2c_scanner_config_t i2c_scanner;

// Helpers required to bind to debounce helper
void eeconfig_read_i2c_scanner(i2c_scanner_config_t *value) {
    eeconfig_read_i2c_scanner_datablock(value, 0, sizeof(i2c_scanner_config_t));
}
void eeconfig_update_i2c_scanner(i2c_scanner_config_t *value) {
    eeconfig_update_i2c_scanner_datablock(value, 0, sizeof(i2c_scanner_config_t));
}

EECONFIG_DEBOUNCE_HELPER(i2c_scanner, i2c_scanner);

static void do_scan(void) {
    if (!i2c_scanner.enabled) {
        return;
    }
    uint8_t nDevices = 0;

    xprintf("Scanning for I2C Devices...\n");

    for (uint8_t address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // i2c_ping_address to see if a device did acknowledge to the address.
        i2c_status_t error = i2c_ping_address(address << 1, I2C_SCANNER_TIMEOUT);
        if (error == I2C_STATUS_SUCCESS) {
            xprintf("  I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else {
            // xprintf("  Unknown error (%u) at address 0x%02X\n", error, address);
        }
    }

    if (nDevices == 0) {
        xprintf("No I2C devices found\n");
    }
}

void housekeeping_task_i2c_scanner(void) {
    static uint16_t scan_timer = 0;

    if (timer_elapsed(scan_timer) > 5000) {
        do_scan();
        scan_timer = timer_read();
    }
    eeconfig_flush_i2c_scanner_task(1000);
    housekeeping_task_i2c_scanner_kb();
}

void keyboard_post_init_i2c_scanner(void) {
    i2c_init();

    if (!eeconfig_is_i2c_scanner_datablock_valid()) {
        eeconfig_init_i2c_scanner_datablock();
        i2c_scanner.enabled = true;
        eeconfig_flush_i2c_scanner(true);
    }

    eeconfig_init_i2c_scanner();

    keyboard_post_init_i2c_scanner_kb();
}

bool i2c_scanner_get_enabled(void) {
    return i2c_scanner.enabled;
}

void i2c_scanner_set_enabled(bool enable) {
    i2c_scanner.enabled = enable;
    eeconfig_flag_i2c_scanner(true);
}
