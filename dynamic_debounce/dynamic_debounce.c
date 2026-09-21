// Copyright 2017 Alex Ong<the.onga@gmail.com>
// Copyright 2020 Andrei Purdea<andrei@purdea.ro>
// Copyright 2021 Simon Arlott
// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Dynamic debounce: wraps every stock QMK symmetric/asymmetric debounce algorithm
// (quantum/debounce/*.c) behind a single dispatcher so the active algorithm can be
// switched at runtime, e.g. via keycodes or from keymap code.

#include QMK_KEYBOARD_H
#include "community_modules.h"
#include "dynamic_debounce.h"
#include "debounce.h"
#include "eeconfig.h"
#include "timer.h"
#include "util.h"
#include <string.h>

#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#    ifndef FORCED_SYNC_THROTTLE_MS
#        define FORCED_SYNC_THROTTLE_MS 100
#    endif // FORCED_SYNC_THROTTLE_MS
#endif     // SPLIT_KEYBOARD

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#if defined(DEBOUNCE)
#    define DYNAMIC_DEBOUNCE_DEFAULT_TIME DEBOUNCE
#else
#    define DYNAMIC_DEBOUNCE_DEFAULT_TIME 5
#endif // DEBOUNCE
#ifndef DYNAMIC_DEBOUNCE_TIME_STEP
#    define DYNAMIC_DEBOUNCE_TIME_STEP 1
#endif

typedef struct {
    void (*init)(void);
    void (*reset)(void);
    bool (*debounce)(matrix_row_t raw[], matrix_row_t cooked[], bool changed);
    const char *name;
} dynamic_debounce_algo_entry_t;

// Runtime debounce time, in milliseconds. 0 disables debouncing entirely.
static uint8_t dynamic_debounce_time = DYNAMIC_DEBOUNCE_DEFAULT_TIME;

// asym_eager_defer_pk stores its counter in a 7-bit bitfield.
static inline uint8_t dynamic_debounce_time_asym(void) {
    return dynamic_debounce_time > 127 ? 127 : dynamic_debounce_time;
}

#define DEBOUNCE_ALGORITHM(name)
#define DEBOUNCE_ALGORITHM_IMPLS
#include "debounce.inc"
#undef DEBOUNCE_ALGORITHM_IMPLS
#undef DEBOUNCE_ALGORITHM

// ---------------------------------------------------------------------------
// Dispatcher
// ---------------------------------------------------------------------------
static const dynamic_debounce_algo_entry_t dynamic_debounce_algorithms[DYNAMIC_DEBOUNCE_ALGO_COUNT] = {
#define DEBOUNCE_ALGORITHM(name) [DYNAMIC_DEBOUNCE_##name] = {name##_init, name##_reset, name##_debounce, #name},
#include "debounce.inc"
#undef DEBOUNCE_ALGORITHM
};

static dynamic_debounce_algo_t current_algo = DYNAMIC_DEBOUNCE_DEFAULT_ALGO;

typedef struct {
    uint8_t algo;
    uint8_t time;
} dynamic_debounce_eeconfig_t;

static dynamic_debounce_eeconfig_t g_dynamic_debounce_eeconfig = {
    .algo = DYNAMIC_DEBOUNCE_DEFAULT_ALGO,
    .time = DYNAMIC_DEBOUNCE_DEFAULT_TIME,
};

_Static_assert(sizeof(dynamic_debounce_eeconfig_t) <= EECONFIG_MODULE_DYNAMIC_DEBOUNCE_DATA_SIZE,
               "EECONFIG_MODULE_DYNAMIC_DEBOUNCE_DATA_SIZE is too small");

void eeconfig_read_dynamic_debounce(dynamic_debounce_eeconfig_t *value) {
    eeconfig_read_dynamic_debounce_datablock(value, 0, sizeof(dynamic_debounce_eeconfig_t));
}

void eeconfig_update_dynamic_debounce(dynamic_debounce_eeconfig_t *value) {
    eeconfig_update_dynamic_debounce_datablock(value, 0, sizeof(dynamic_debounce_eeconfig_t));
}

EECONFIG_DEBOUNCE_HELPER(dynamic_debounce, g_dynamic_debounce_eeconfig);

void debounce_init(void) {
    for (uint8_t i = 0; i < DYNAMIC_DEBOUNCE_ALGO_COUNT; i++) {
        dynamic_debounce_algorithms[i].init();
    }
}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    if (dynamic_debounce_time == 0) {
        return NONE_debounce(raw, cooked, changed);
    }
    return dynamic_debounce_algorithms[current_algo].debounce(raw, cooked, changed);
}

uint8_t dynamic_debounce_get_time(void) {
    return dynamic_debounce_time;
}

void dynamic_debounce_set_time(uint8_t time_ms) {
    dynamic_debounce_time = time_ms;
    dynamic_debounce_algorithms[current_algo].reset();
    g_dynamic_debounce_eeconfig.time = time_ms;
    eeconfig_flag_dynamic_debounce(true);
}

void dynamic_debounce_increase_time(void) {
    uint16_t new_time = (uint16_t)dynamic_debounce_time + DYNAMIC_DEBOUNCE_TIME_STEP;
    dynamic_debounce_set_time(new_time > UINT8_MAX ? UINT8_MAX : (uint8_t)new_time);
}

void dynamic_debounce_decrease_time(void) {
    dynamic_debounce_set_time(
        dynamic_debounce_time > DYNAMIC_DEBOUNCE_TIME_STEP ? dynamic_debounce_time - DYNAMIC_DEBOUNCE_TIME_STEP : 0);
}

dynamic_debounce_algo_t dynamic_debounce_get_algorithm(void) {
    return current_algo;
}

void dynamic_debounce_set_algorithm(dynamic_debounce_algo_t algo) {
    if (algo >= DYNAMIC_DEBOUNCE_ALGO_COUNT) {
        return;
    }
    current_algo = algo;
    dynamic_debounce_algorithms[current_algo].reset();
    g_dynamic_debounce_eeconfig.algo = (uint8_t)current_algo;
    eeconfig_flag_dynamic_debounce(true);
}

void dynamic_debounce_next_algorithm(void) {
    dynamic_debounce_set_algorithm((current_algo + 1) % DYNAMIC_DEBOUNCE_ALGO_COUNT);
}

void dynamic_debounce_previous_algorithm(void) {
    dynamic_debounce_set_algorithm((current_algo + DYNAMIC_DEBOUNCE_ALGO_COUNT - 1) % DYNAMIC_DEBOUNCE_ALGO_COUNT);
}

const char *dynamic_debounce_get_algorithm_name(dynamic_debounce_algo_t algo) {
    if (algo >= DYNAMIC_DEBOUNCE_ALGO_COUNT) {
        return "unknown";
    }

    static char    buf[20]   = {0};
    static uint8_t last_algo = 0;
    if (last_algo != algo) {
        last_algo = algo;
        snprintf(buf, sizeof(buf), "%s", dynamic_debounce_algorithms[algo].name);
        for (uint8_t i = 1; i < sizeof(buf); ++i) {
            if (buf[i] == 0)
                break;
            else if (buf[i] == '_')
                buf[i] = ' ';
            else if (buf[i - 1] == ' ')
                buf[i] = toupper(buf[i]);
            else if (buf[i - 1] != ' ')
                buf[i] = tolower(buf[i]);
        }
    }
    return buf;
}

#ifdef SPLIT_KEYBOARD
typedef struct {
    uint8_t algo;
    uint8_t time;
} dynamic_debounce_sync_t;

_Static_assert(sizeof(dynamic_debounce_sync_t) <= RPC_M2S_BUFFER_SIZE,
               "Dynamic debounce sync message size exceeds buffer size!");

void dynamic_debounce_sync_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                   uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    const dynamic_debounce_sync_t *sync_data = (const dynamic_debounce_sync_t *)initiator2target_buffer;
    if (sync_data->algo != (uint8_t)current_algo) {
        dynamic_debounce_set_algorithm((dynamic_debounce_algo_t)sync_data->algo);
    }
    if (sync_data->time != dynamic_debounce_time) {
        dynamic_debounce_set_time(sync_data->time);
    }
}

// Pushes the master's current algorithm/time to the other half.
void dynamic_debounce_sync(void) {
    static dynamic_debounce_sync_t last_synced_state;
#    if FORCED_SYNC_THROTTLE_MS > 0
    static uint16_t last_sync_time;
#    endif

    dynamic_debounce_sync_t current_state = {
        .algo = (uint8_t)current_algo,
        .time = dynamic_debounce_time,
    };

    if (memcmp(&current_state, &last_synced_state, sizeof(current_state)) != 0
#    if FORCED_SYNC_THROTTLE_MS > 0
        || timer_elapsed(last_sync_time) >= FORCED_SYNC_THROTTLE_MS
#    endif
    ) {
        last_synced_state = current_state;
        if (transaction_rpc_send(RPC_ID_DYNAMIC_DEBOUNCE_SYNC, sizeof(current_state), &current_state)) {
#    if FORCED_SYNC_THROTTLE_MS > 0
            last_sync_time = timer_read();
#    endif
        }
    }
}
#endif // SPLIT_KEYBOARD

void keyboard_post_init_dynamic_debounce(void) {
    eeconfig_init_dynamic_debounce();

    current_algo = (dynamic_debounce_algo_t)g_dynamic_debounce_eeconfig.algo;
    if (current_algo >= DYNAMIC_DEBOUNCE_ALGO_COUNT) {
        current_algo = DYNAMIC_DEBOUNCE_DEFAULT_ALGO;
    }
    dynamic_debounce_time = g_dynamic_debounce_eeconfig.time;
    dynamic_debounce_algorithms[current_algo].reset();

#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_DYNAMIC_DEBOUNCE_SYNC, dynamic_debounce_sync_handler);
#endif // SPLIT_KEYBOARD
    keyboard_post_init_dynamic_debounce_kb();
}

#ifndef DYNAMIC_DEBOUNCE_EECONFIG_FLUSH_MS
#    define DYNAMIC_DEBOUNCE_EECONFIG_FLUSH_MS 500
#endif

void housekeeping_task_dynamic_debounce(void) {
#ifdef SPLIT_KEYBOARD
    if (is_keyboard_master()) {
        dynamic_debounce_sync();
    }
#endif // SPLIT_KEYBOARD
    eeconfig_flush_dynamic_debounce_task(DYNAMIC_DEBOUNCE_EECONFIG_FLUSH_MS);
}

void eeconfig_init_dynamic_debounce_datablock(void) {
    g_dynamic_debounce_eeconfig.algo = DYNAMIC_DEBOUNCE_DEFAULT_ALGO;
    g_dynamic_debounce_eeconfig.time = DYNAMIC_DEBOUNCE_DEFAULT_TIME;
    dynamic_debounce_set_algorithm((dynamic_debounce_algo_t)g_dynamic_debounce_eeconfig.algo);
    dynamic_debounce_set_time(g_dynamic_debounce_eeconfig.time);
    eeconfig_flush_dynamic_debounce(true);
}

bool process_record_dynamic_debounce(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case CM_DYNAMIC_DEBOUNCE_NEXT:
                dynamic_debounce_next_algorithm();
                return false;
            case CM_DYNAMIC_DEBOUNCE_PREV:
                dynamic_debounce_previous_algorithm();
                return false;
            case CM_DYNAMIC_DEBOUNCE_TIME_UP:
                dynamic_debounce_increase_time();
                return false;
            case CM_DYNAMIC_DEBOUNCE_TIME_DOWN:
                dynamic_debounce_decrease_time();
                return false;
        }
    }
    return true;
}
