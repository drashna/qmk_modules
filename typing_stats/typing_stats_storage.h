// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/**
 * @file typing_stats_storage.h
 * @brief Storage Package - Internal API
 *
 * This package handles EEPROM storage and persistence of typing statistics.
 * External code should not use these functions directly.
 *
 * @warning This is an internal header - do not include directly
 */

#include "typing_stats_private.h"

// Storage module API
void ts_storage_load(ts_blob_t *blob);
void ts_storage_save(const ts_blob_t *blob);
void ts_storage_mark_dirty(void);
void ts_storage_force_flush(void);
void ts_storage_task(void); // Call periodically for auto-flush

// Storage status
bool     ts_storage_is_dirty(void);
uint32_t ts_storage_get_last_flush_time(void);
