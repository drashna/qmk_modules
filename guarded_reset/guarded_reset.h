// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "action.h"

typedef bool (*guarded_reset_handler_t)(uint16_t keycode, keyrecord_t *record);

typedef struct {
    uint16_t                 keycode;
    guarded_reset_handler_t  handler;
} guarded_reset_entry_t;

uint16_t get_guarded_reset_hold_ms(void);
void set_guarded_reset_hold_ms(uint16_t ms);
void guarded_reset_hold_ms_step(bool increase);
