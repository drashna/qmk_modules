// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>

float get_drag_scroll_h_divisor(void);
float get_drag_scroll_v_divisor(void);
void  set_drag_scroll_h_divisor(float divisor);
void  set_drag_scroll_v_divisor(float divisor);
void  set_drag_scroll_divisor(float divisor);
bool  get_drag_scroll_scrolling(void);
void  set_drag_scroll_scrolling(bool scrolling);
