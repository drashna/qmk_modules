// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>
#include <tmk_core/protocol/report.h>

bool is_wiggle_ball_enabled(void);
void set_wiggle_ball_timeout(uint16_t timeout);
uint16_t get_wiggle_ball_timeout(void);
void set_wiggle_ball_direction_switch_timeout(uint16_t timeout);
uint16_t get_wiggle_ball_direction_switch_timeout(void);
void wiggle_ball_state_change(bool enabled, report_mouse_t *report);
