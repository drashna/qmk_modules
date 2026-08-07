// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2025 dmyoung <@dmyoung9>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wpm_stats.h"
#include <quantum.h>
#if defined(SPLIT_KEYBOARD)
#    include "transactions.h"
#    ifndef FORCED_SYNC_THROTTLE_MS
#        define FORCED_SYNC_THROTTLE_MS 1000
#    endif
#endif

#ifdef SPLIT_KEYBOARD
ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 1, 2);
#else
ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);
#endif

static bool     g_initialized = false;
static uint16_t g_last_update;
static uint16_t g_max_wpm   = 0;
static uint32_t g_wpm_sum   = 0;
static uint16_t g_wpm_count = 0;

#ifdef SPLIT_KEYBOARD

/**
 * @brief Structure to hold WPM statistics for synchronization between halves of a split keyboard
 *
 */
typedef struct wpm_stat_config_t {
    uint16_t max_wpm;
    uint32_t wpm_sum;
    uint16_t wpm_count;
} wpm_stat_config_t;

/**
 * @brief Handler for receiving WPM statistics synchronization from the other half of a split keyboard
 * @param initiator2target_buffer_size Size of the buffer sent from the initiator to the target
 * @param initiator2target_buffer Buffer containing the WPM statistics sent from the initiator
 * @param target2initiator_buffer_size Size of the buffer sent from the target to the initiator (not used in this
 * handler)
 * @param target2initiator_buffer Buffer to send
 */
static void wpm_state_sync_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                   uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    static wpm_stat_config_t wpm_stat_config;

    if (memcmp(initiator2target_buffer, &wpm_stat_config, sizeof(wpm_stat_config_t)) != 0) {
        memcpy(&wpm_stat_config, initiator2target_buffer, sizeof(wpm_stat_config_t));
        g_max_wpm   = wpm_stat_config.max_wpm;
        g_wpm_sum   = wpm_stat_config.wpm_sum;
        g_wpm_count = wpm_stat_config.wpm_count;
    }
}
#endif // SPLIT_KEYBOARD

void wpm_stats_init(void) {
    g_initialized = true;
    g_max_wpm     = 0;
    g_wpm_sum     = 0;
    g_wpm_count   = 0;
    g_last_update = timer_read();
}

void keyboard_pre_init_wpm_stats(void) {
    wpm_stats_init();

#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_WPM_STATS_SYNC, wpm_state_sync_handler);
#endif // SPLIT_KEYBOARD

    keyboard_pre_init_wpm_stats_kb();
}

void housekeeping_task_wpm_stats(void) {
    if (!g_initialized || !is_keyboard_master()) goto done;

    if (timer_elapsed(g_last_update) >= 1000) { // Update every second
        uint16_t current_wpm = wpm_stats_get_current();

        // Only track when actively typing (WPM > 0)
        if (current_wpm > 0) {
            // Update max WPM
            if (current_wpm > g_max_wpm) {
                g_max_wpm = current_wpm;
            }

            // Update running average (prevent overflow)
            if (g_wpm_count < 1000) {
                g_wpm_sum += current_wpm;
                g_wpm_count++;
            } else {
                // Reset to prevent overflow, keeping recent average
                g_wpm_sum   = (g_wpm_sum / g_wpm_count) * 100 + current_wpm;
                g_wpm_count = 101;
            }
        }

        g_last_update = timer_read();
    }

#ifdef SPLIT_KEYBOARD
    if (is_keyboard_master()) {
        // Sync WPM stats to the other half of the keyboard
        bool                     needs_sync           = false;
        static uint16_t          last_sync            = 0;
        static wpm_stat_config_t last_wpm_stat_config = {0};
        wpm_stat_config_t        wpm_stat_config      = {
            .max_wpm   = g_max_wpm,
            .wpm_sum   = g_wpm_sum,
            .wpm_count = g_wpm_count,
        };

        if (memcmp(&wpm_stat_config, &last_wpm_stat_config, sizeof(wpm_stat_config_t)) != 0) {
            memcpy(&last_wpm_stat_config, &wpm_stat_config, sizeof(wpm_stat_config_t));
            needs_sync = true;
        }

        if (timer_elapsed(last_sync) > FORCED_SYNC_THROTTLE_MS || needs_sync) {
            if (transaction_rpc_send(RPC_ID_WPM_STATS_SYNC, sizeof(wpm_stat_config_t), &wpm_stat_config)) {
                last_sync = timer_read();
            }
        }
    }
#endif // SPLIT_KEYBOARD

done:
    housekeeping_task_wpm_stats_kb();
}

bool wpm_stats_get(wpm_stats_t *stats) {
    if (!stats || !g_initialized) {
        return false;
    }

    stats->current_wpm     = wpm_stats_get_current();
    stats->average_wpm     = wpm_stats_get_avg();
    stats->session_max_wpm = wpm_stats_get_max();

    return true;
}

uint16_t wpm_stats_get_current(void) {
    if (!g_initialized) return UINT16_MAX;

    return get_current_wpm();
}

uint16_t wpm_stats_get_avg(void) {
    if (!g_initialized) return UINT16_MAX;

    return (g_wpm_count > 0) ? (uint16_t)(g_wpm_sum / g_wpm_count) : 0;
}

uint16_t wpm_stats_get_max(void) {
    if (!g_initialized) return UINT16_MAX;

    return g_max_wpm;
}
