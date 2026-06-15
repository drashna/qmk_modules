// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Gets the horizontal drag-scroll divisor.
 *
 * @return int8_t The horizontal divisor.
 */
int8_t get_drag_scroll_h_divisor(void);

/**
 * @brief Gets the vertical drag-scroll divisor.
 *
 * @return int8_t The vertical divisor.
 */
int8_t get_drag_scroll_v_divisor(void);

/**
 * @brief Sets the horizontal drag-scroll divisor.
 *
 * @param divisor The new horizontal divisor.
 */
void   set_drag_scroll_h_divisor(int8_t divisor);

/**
 * @brief Sets the vertical drag-scroll divisor.
 *
 * @param divisor The new vertical divisor.
 */
void   set_drag_scroll_v_divisor(int8_t divisor);

/**
 * @brief Sets both drag-scroll divisors to the same value.
 *
 * @param divisor The divisor to apply to both axes.
 */
void   set_drag_scroll_divisor(int8_t divisor);

/**
 * @brief Gets whether drag scrolling is enabled.
 *
 * @return true Drag scrolling is enabled.
 * @return false Drag scrolling is disabled.
 */
bool   get_drag_scroll_scrolling(void);

/**
 * @brief Enables or disables drag scrolling.
 *
 * @param scrolling true to enable drag scrolling, false to disable it.
 */
void   set_drag_scroll_scrolling(bool scrolling);
