// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ft6x36.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"
#include "gpio.h"
#include "wait.h"

// FT6X36 touch controller datasheet:
// https://www.buydisplay.com/download/ic/FT6236-FT6336-FT6436L-FT6436_Datasheet.pdf

#ifndef FT6X36_I2C_ADDRRESS
#    define FT6X36_I2C_ADDRRESS 0x38
#endif // FT6X36_I2C_ADDRRESS
#ifndef FT6X36_I2C_TIMEOUT
#    define FT6X36_I2C_TIMEOUT 100
#endif // FT6X36_I2C_TIMEOUT

// #ifndef FT6X36_RESET_PIN
// #    error "FT6X36_RESET_PIN is not defined and is required for proper operation"
// #endif // FT6X36_RESET_PIN

static const uint8_t threshold = FT6X36_DEFAULT_THRESHOLD;

bool ft6x36_init(void) {
    i2c_init();
    uint8_t data = 0;

#ifdef FT6X36_INTERRUPT_PIN
    gpio_set_pin_input_low(FT6X36_INTERRUPT_PIN);
    gpio_set_pin_output_open_drain(FT6X36_RESET_PIN);
#endif // FT6X36_INTERRUPT_PIN
#ifndef FT6X36_RESET_PIN
    gpio_write_pin_high(FT6X36_RESET_PIN);
#endif // FT6X36_RESET_PIN

    if (i2c_read_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_PANEL_ID, &data, 1, FT6X36_I2C_TIMEOUT) !=
        I2C_STATUS_SUCCESS) {
        xprintf("Failed to read panel ID\n");
        return false;
    }
    if (data != FT6X36_VENDID) {
        xprintf("Invalid vendor ID: %02X\n", data);
        return false;
    }

    i2c_read_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_CHIPID, &data, sizeof(data), FT6X36_I2C_TIMEOUT);
    if (data == FT6206_CHIPID) {
        xprintf("Detected FT6206\n");
    } else if (data == FT6236_CHIPID) {
        xprintf("Detected FT6236\n");
    } else if (data == FT6336_CHIPID) {
        xprintf("Detected FT6336\n");
    } else {
        xprintf("Invalid chip ID: %02X\n", data);
        return false;
    }

    // set device mode to "working mode"
    i2c_write_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_DEVICE_MODE, 0, 1, FT6X36_I2C_TIMEOUT);
    // set threshold for touch detection
    i2c_write_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_THRESHOLD, &threshold, 1, FT6X36_I2C_TIMEOUT);
    // set report rate in active mode
    i2c_write_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_TOUCHRATE_ACTIVE, (const uint8_t *)0x0E, 1,
                       FT6X36_I2C_TIMEOUT);
    // // set to polling mode
    i2c_write_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_INTERRUPT_MODE, (const uint8_t *)1, 1, FT6X36_I2C_TIMEOUT);

    return true;
}

void ft6x36_read_data(void) {
    uint8_t data[16];
    i2c_write_register(FT6X36_I2C_ADDRRESS << 1, FT6X36_REG_DEVICE_MODE, 0, 1, FT6X36_I2C_TIMEOUT);
    i2c_status_t status = i2c_read_register(FT6X36_I2C_ADDRRESS << 1, 0, data, ARRAY_SIZE(data), FT6X36_I2C_TIMEOUT);

    if (status != I2C_STATUS_SUCCESS) {
        xprintf("Failed to read data\n");
        return;
    }
    xprintf("Data 1: %02X %02X %02X %02X %02X %02X %02X %02X\n", data[0], data[1], data[2], data[3], data[4], data[5],
            data[6], data[7]);
    xprintf("Data 2: %02X %02X %02X %02X %02X %02X %02X %02X\n", data[8], data[9], data[10], data[11], data[12],
            data[13], data[14], data[15]);
}

void ft6x36_task(void) {
    static uint16_t interval = 0;
    if (timer_elapsed(interval) > 1000) {
        interval = timer_read();
#ifdef FT6X36_INTERRUPT_PIN
        // bool state = gpio_read_pin(FT6X36_INTERRUPT_PIN);
        // xprintf("INT: %d\n", state);
#endif // FT6X36_INTERRUPT_PIN
        ft6x36_read_data();
    }
}
