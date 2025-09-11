// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int8_t get_drag_scroll_h_divisor(void);
int8_t get_drag_scroll_v_divisor(void);
void   set_drag_scroll_h_divisor(int8_t divisor);
void   set_drag_scroll_v_divisor(int8_t divisor);
void   set_drag_scroll_divisor(int8_t divisor);
bool   get_drag_scroll_scrolling(void);
void   set_drag_scroll_scrolling(bool scrolling);
