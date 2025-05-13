// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "quantum.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#include "i2c_master.h"
#include "debug.h"

static bool i2c_scanner_enable = true;

#ifndef I2C_SCANNER_TIMEOUT
#    define I2C_SCANNER_TIMEOUT 50
#endif // I2C_SCANNER_TIMEOUT

static void do_scan(void) {
    if (!i2c_scanner_enable) {
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

uint16_t scan_timer = 0;

void housekeeping_task_i2c_scanner(void) {
    if (timer_elapsed(scan_timer) > 5000) {
        do_scan();
        scan_timer = timer_read();
    }
    housekeeping_task_i2c_scanner_kb();
}

void keyboard_post_init_i2c_scanner(void) {
    i2c_init();
    scan_timer = timer_read();
    keyboard_post_init_i2c_scanner_kb();
}

bool i2c_scanner_get_enabled(void) {
    return i2c_scanner_enable;
}

void i2c_scanner_set_enabled(bool enable) {
    i2c_scanner_enable = enable;
}
