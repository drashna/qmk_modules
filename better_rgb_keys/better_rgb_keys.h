// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

/**
 * @brief Returns the current interval between HSV adjustment steps, in milliseconds.
 *
 * Smaller values make the colour change faster when a key is held.
 * The default is @c BETTER_RGB_TICK_MS (15 ms).
 */
uint16_t better_rgb_get_tick(void);

/**
 * @brief Sets the interval between HSV adjustment steps, in milliseconds.
 *
 * @param tick_ms  New tick interval in milliseconds. Must be > 0.
 */
void better_rgb_set_tick(uint16_t tick_ms);
