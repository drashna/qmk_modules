// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later


#pragma once

#include <stdbool.h>
#include <stdint.h>

void velocikey_toggle(void);
void velocikey_enable(void);
void velocikey_disable(void);
bool velocikey_get_enabled(void);
void velocikey_accelerate(void);
void velocikey_decelerate(void);
void velocikey_set_max_speed(uint8_t max_speed);
uint8_t velocikey_get_max_speed(void);
void velocikey_set_min_speed(uint8_t min_speed);
uint8_t velocikey_get_min_speed(void);
