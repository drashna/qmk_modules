// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>

bool console_keylogging_get_enabled(void);
void console_keylogging_set_enabled(bool enable);
void console_keylogging_print_handler(uint16_t keycode, keyrecord_t *record);
