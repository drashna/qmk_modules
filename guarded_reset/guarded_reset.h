// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

/**
 * @brief Compile-time default duration in milliseconds a guarded keycode must
 *        be held before it triggers.  Override in your config.h to customise.
 *
 * Default: 3000 ms (3 seconds)
 */
#ifndef GUARDED_RESET_HOLD_MS
#    define GUARDED_RESET_HOLD_MS 3000
#endif


/** @brief Get the current hold duration in milliseconds. */
uint16_t get_guarded_reset_hold_ms(void);

/**
 * @brief Set the hold duration in milliseconds at runtime.
 *        Valid range: 1–65535 ms. Passing 0 resets to the compiled default
 *        (GUARDED_RESET_HOLD_MS).
 */
void set_guarded_reset_hold_ms(uint16_t ms);
