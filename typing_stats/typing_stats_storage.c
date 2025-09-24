// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_storage.h"
#include "eeconfig.h"
#include "eeprom.h"
#include "timer.h"
#include <string.h>

// EEPROM address for typing stats data (use a safe offset in user space)
#define TS_EEPROM_ADDR 0x200

// Internal state
static bool     g_dirty      = false;
static uint32_t g_last_flush = 0;

// Forward declarations
static uint32_t crc32_update(uint32_t crc, const uint8_t *data, uint32_t len);
static void     ts_storage_default_blob(ts_blob_t *blob);

// Storage API implementation
void ts_storage_load(ts_blob_t *blob) {
    if (!blob) return;

    // Read entire blob from EEPROM user data
    eeprom_read_block(blob, (void *)TS_EEPROM_ADDR, sizeof(*blob));

    // Validate magic and version
    bool valid = (blob->magic == TS_MAGIC && blob->version == TS_VERSION);

    // Validate CRC if magic/version are correct
    if (valid) {
        uint32_t expected_crc   = blob->crc32;
        blob->crc32             = 0; // Zero out CRC field for calculation
        uint32_t calculated_crc = crc32_update(0xFFFFFFFFu, (const uint8_t *)blob, sizeof(*blob));
        blob->crc32             = expected_crc; // Restore original CRC
        valid                   = (calculated_crc == expected_crc);
    }

    // Use defaults if validation failed
    if (!valid) {
        ts_storage_default_blob(blob);
        ts_storage_mark_dirty();
        ts_storage_force_flush();
    }
}

void ts_storage_save(const ts_blob_t *blob) {
    if (!blob) return;

    // Create a copy for CRC calculation
    ts_blob_t temp_blob = *blob;

    // Calculate CRC with CRC field zeroed
    temp_blob.crc32 = 0;
    uint32_t crc    = crc32_update(0xFFFFFFFFu, (const uint8_t *)&temp_blob, sizeof(temp_blob));
    temp_blob.crc32 = crc;

    // Write to EEPROM
    eeprom_update_block(&temp_blob, (void *)TS_EEPROM_ADDR, sizeof(temp_blob));

    // Update internal state
    g_dirty      = false;
    g_last_flush = timer_read32();
}

void ts_storage_mark_dirty(void) {
    g_dirty = true;
}

void ts_storage_force_flush(void) {
    if (!ts_core_is_initialized()) return;

    ts_blob_t *blob = ts_core_get_blob_ptr();
    ts_storage_save(blob);
}

void ts_storage_task(void) {
    if (!g_dirty || !ts_core_is_initialized()) return;

    uint32_t now         = timer_read32();
    uint32_t event_count = ts_core_get_event_counter();

    // Check if we should flush based on time or event count
    bool should_flush = (now - g_last_flush) >= (TS_FLUSH_SECONDS * 1000UL) || event_count >= TS_FLUSH_EVENTS;

    if (should_flush) {
        ts_storage_force_flush();
    }
}

bool ts_storage_is_dirty(void) {
    return g_dirty;
}

uint32_t ts_storage_get_last_flush_time(void) {
    return g_last_flush;
}

// Internal helper functions
static void ts_storage_default_blob(ts_blob_t *blob) {
    memset(blob, 0, sizeof(*blob));
    blob->magic   = TS_MAGIC;
    blob->version = TS_VERSION;
}

// Simple CRC-32 implementation (polynomial 0xEDB88320)
static uint32_t crc32_update(uint32_t crc, const uint8_t *data, uint32_t len) {
    crc = ~crc;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (-(int32_t)(crc & 1)));
        }
    }
    return ~crc;
}
