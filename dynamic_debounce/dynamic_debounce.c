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

// ---------------------------------------------------------------------------
// none: passthrough, no debouncing at all
// ---------------------------------------------------------------------------
static void none_init(void) {}
static void none_reset(void) {}
static bool none_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool cooked_changed = false;
    if (changed) {
        size_t matrix_size = MATRIX_ROWS_PER_HAND * sizeof(matrix_row_t);
        if (memcmp(cooked, raw, matrix_size) != 0) {
            memcpy(cooked, raw, matrix_size);
            cooked_changed = true;
        }
    }
    return cooked_changed;
}

// ---------------------------------------------------------------------------
// sym_defer_g: global symmetric defer
// ---------------------------------------------------------------------------
static fast_timer_t g_debouncing_time;
static bool         g_debouncing;

static void g_init(void) {}
static void g_reset(void) {
    g_debouncing = false;
}

static bool g_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool cooked_changed = false;

    if (changed) {
        g_debouncing      = true;
        g_debouncing_time = timer_read_fast();
    } else if (g_debouncing && timer_elapsed_fast(g_debouncing_time) >= dynamic_debounce_time) {
        size_t matrix_size = MATRIX_ROWS_PER_HAND * sizeof(matrix_row_t);
        if (memcmp(cooked, raw, matrix_size) != 0) {
            memcpy(cooked, raw, matrix_size);
            cooked_changed = true;
        }
        g_debouncing = false;
    }

    return cooked_changed;
}

// ---------------------------------------------------------------------------
// sym_eager_pr: per-row symmetric eager
// ---------------------------------------------------------------------------
static uint8_t      epr_counters[MATRIX_ROWS_PER_HAND];
static bool         epr_counters_need_update;
static bool         epr_matrix_need_update;
static bool         epr_cooked_changed;
static fast_timer_t epr_last_time;

static void epr_init(void) {}
static void epr_reset(void) {
    memset(epr_counters, 0, sizeof(epr_counters));
    epr_counters_need_update = false;
    epr_matrix_need_update   = false;
    epr_cooked_changed       = false;
}

static inline void epr_update_counters(uint8_t elapsed_time) {
    epr_counters_need_update = false;
    epr_matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (epr_counters[row] != 0) {
            if (epr_counters[row] <= elapsed_time) {
                epr_counters[row]     = 0;
                epr_matrix_need_update = true;
            } else {
                epr_counters[row] -= elapsed_time;
                epr_counters_need_update = true;
            }
        }
    }
}

static inline void epr_transfer(matrix_row_t raw[], matrix_row_t cooked[]) {
    epr_matrix_need_update = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        matrix_row_t existing_row = cooked[row];
        matrix_row_t raw_row      = raw[row];

        if (existing_row != raw_row && epr_counters[row] == 0) {
            epr_counters[row]        = dynamic_debounce_time;
            epr_cooked_changed |= cooked[row] ^ raw_row;
            cooked[row]              = raw_row;
            epr_counters_need_update = true;
        }
    }
}

static bool epr_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool updated_last  = false;
    epr_cooked_changed = false;

    if (epr_counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, epr_last_time);
        epr_last_time             = now;
        updated_last              = true;

        if (elapsed_time > 0) {
            epr_update_counters(MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed || epr_matrix_need_update) {
        if (!updated_last) {
            epr_last_time = timer_read_fast();
        }
        epr_transfer(raw, cooked);
    }

    return epr_cooked_changed;
}

// ---------------------------------------------------------------------------
// sym_defer_pr: per-row symmetric defer
// ---------------------------------------------------------------------------
static uint8_t      dpr_counters[MATRIX_ROWS_PER_HAND];
static bool         dpr_counters_need_update;
static bool         dpr_cooked_changed;
static fast_timer_t dpr_last_time;

static void dpr_init(void) {}
static void dpr_reset(void) {
    memset(dpr_counters, 0, sizeof(dpr_counters));
    dpr_counters_need_update = false;
    dpr_cooked_changed       = false;
}

static inline void dpr_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    dpr_counters_need_update = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (dpr_counters[row] != 0) {
            if (dpr_counters[row] <= elapsed_time) {
                dpr_counters[row] = 0;
                dpr_cooked_changed |= cooked[row] ^ raw[row];
                cooked[row] = raw[row];
            } else {
                dpr_counters[row] -= elapsed_time;
                dpr_counters_need_update = true;
            }
        }
    }
}

static inline void dpr_start_counters(matrix_row_t raw[], matrix_row_t cooked[]) {
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (raw[row] != cooked[row]) {
            dpr_counters[row]        = dynamic_debounce_time;
            dpr_counters_need_update = true;
        } else {
            dpr_counters[row] = 0;
        }
    }
}

static bool dpr_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool updated_last   = false;
    dpr_cooked_changed  = false;

    if (dpr_counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, dpr_last_time);
        dpr_last_time             = now;
        updated_last              = true;

        if (elapsed_time > 0) {
            dpr_update_and_transfer(raw, cooked, MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed) {
        if (!updated_last) {
            dpr_last_time = timer_read_fast();
        }
        dpr_start_counters(raw, cooked);
    }

    return dpr_cooked_changed;
}

// ---------------------------------------------------------------------------
// sym_eager_pk: per-key symmetric eager
// ---------------------------------------------------------------------------
static uint8_t      epk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS];
static bool         epk_counters_need_update;
static bool         epk_matrix_need_update;
static bool         epk_cooked_changed;
static fast_timer_t epk_last_time;

static void epk_init(void) {}
static void epk_reset(void) {
    memset(epk_counters, 0, sizeof(epk_counters));
    epk_counters_need_update = false;
    epk_matrix_need_update   = false;
    epk_cooked_changed       = false;
}

static inline void epk_update_counters(uint8_t elapsed_time) {
    epk_counters_need_update = false;
    epk_matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;
            if (epk_counters[index] != 0) {
                if (epk_counters[index] <= elapsed_time) {
                    epk_counters[index]   = 0;
                    epk_matrix_need_update = true;
                } else {
                    epk_counters[index] -= elapsed_time;
                    epk_counters_need_update = true;
                }
            }
        }
    }
}

static inline void epk_transfer(matrix_row_t raw[], matrix_row_t cooked[]) {
    epk_matrix_need_update = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset   = row * MATRIX_COLS;
        matrix_row_t delta        = raw[row] ^ cooked[row];
        matrix_row_t existing_row = cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t     index    = row_offset + col;
            matrix_row_t col_mask = (MATRIX_ROW_SHIFTER << col);

            if ((delta & col_mask) && epk_counters[index] == 0) {
                epk_counters[index]      = dynamic_debounce_time;
                epk_counters_need_update = true;
                existing_row ^= col_mask;
                epk_cooked_changed = true;
            }
        }
        cooked[row] = existing_row;
    }
}

static bool epk_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool updated_last  = false;
    epk_cooked_changed = false;

    if (epk_counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, epk_last_time);
        epk_last_time             = now;
        updated_last              = true;

        if (elapsed_time > 0) {
            epk_update_counters(MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed || epk_matrix_need_update) {
        if (!updated_last) {
            epk_last_time = timer_read_fast();
        }
        epk_transfer(raw, cooked);
    }

    return epk_cooked_changed;
}

// ---------------------------------------------------------------------------
// sym_defer_pk: per-key symmetric defer
// ---------------------------------------------------------------------------
static uint8_t      dpk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS];
static bool         dpk_counters_need_update;
static bool         dpk_cooked_changed;
static fast_timer_t dpk_last_time;

static void dpk_init(void) {}
static void dpk_reset(void) {
    memset(dpk_counters, 0, sizeof(dpk_counters));
    dpk_counters_need_update = false;
    dpk_cooked_changed       = false;
}

static inline void dpk_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    dpk_counters_need_update = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;
            if (dpk_counters[index] != 0) {
                if (dpk_counters[index] <= elapsed_time) {
                    dpk_counters[index]      = 0;
                    matrix_row_t col_mask    = (MATRIX_ROW_SHIFTER << col);
                    matrix_row_t cooked_next = (cooked[row] & ~col_mask) | (raw[row] & col_mask);
                    dpk_cooked_changed |= cooked[row] ^ cooked_next;
                    cooked[row] = cooked_next;
                } else {
                    dpk_counters[index] -= elapsed_time;
                    dpk_counters_need_update = true;
                }
            }
        }
    }
}

static inline void dpk_start_counters(matrix_row_t raw[], matrix_row_t cooked[]) {
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset = row * MATRIX_COLS;
        matrix_row_t delta      = raw[row] ^ cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            if (delta & (MATRIX_ROW_SHIFTER << col)) {
                if (dpk_counters[index] == 0) {
                    dpk_counters[index]      = dynamic_debounce_time;
                    dpk_counters_need_update = true;
                }
            } else {
                dpk_counters[index] = 0;
            }
        }
    }
}

static bool dpk_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool updated_last  = false;
    dpk_cooked_changed = false;

    if (dpk_counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, dpk_last_time);
        dpk_last_time             = now;
        updated_last              = true;

        if (elapsed_time > 0) {
            dpk_update_and_transfer(raw, cooked, MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed) {
        if (!updated_last) {
            dpk_last_time = timer_read_fast();
        }
        dpk_start_counters(raw, cooked);
    }

    return dpk_cooked_changed;
}

// ---------------------------------------------------------------------------
// asym_eager_defer_pk: per-key asymmetric eager press / defer release
// ---------------------------------------------------------------------------
typedef struct {
    bool    pressed : 1;
    uint8_t time : 7;
} aepdpk_counter_t;

static aepdpk_counter_t aepdpk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS];
static bool             aepdpk_counters_need_update;
static bool             aepdpk_matrix_need_update;
static bool             aepdpk_cooked_changed;
static fast_timer_t     aepdpk_last_time;

static void aepdpk_init(void) {}
static void aepdpk_reset(void) {
    memset(aepdpk_counters, 0, sizeof(aepdpk_counters));
    aepdpk_counters_need_update = false;
    aepdpk_matrix_need_update   = false;
    aepdpk_cooked_changed       = false;
}

static inline void aepdpk_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    aepdpk_counters_need_update = false;
    aepdpk_matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;
            if (aepdpk_counters[index].time == 0) {
                continue;
            }
            if (aepdpk_counters[index].time <= elapsed_time) {
                aepdpk_counters[index].time = 0;
                if (aepdpk_counters[index].pressed) {
                    aepdpk_matrix_need_update = true;
                } else {
                    matrix_row_t col_mask    = (MATRIX_ROW_SHIFTER << col);
                    matrix_row_t cooked_next = (cooked[row] & ~col_mask) | (raw[row] & col_mask);
                    aepdpk_cooked_changed |= cooked_next ^ cooked[row];
                    cooked[row] = cooked_next;
                }
            } else {
                aepdpk_counters[index].time -= elapsed_time;
                aepdpk_counters_need_update = true;
            }
        }
    }
}

static inline void aepdpk_transfer(matrix_row_t raw[], matrix_row_t cooked[]) {
    aepdpk_matrix_need_update = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset = row * MATRIX_COLS;
        matrix_row_t delta      = raw[row] ^ cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t     index    = row_offset + col;
            matrix_row_t col_mask = (MATRIX_ROW_SHIFTER << col);

            if (delta & col_mask) {
                if (aepdpk_counters[index].time == 0) {
                    aepdpk_counters[index].pressed = (raw[row] & col_mask);
                    aepdpk_counters[index].time    = dynamic_debounce_time_asym();
                    aepdpk_counters_need_update    = true;

                    if (aepdpk_counters[index].pressed) {
                        cooked[row] ^= col_mask;
                        aepdpk_cooked_changed = true;
                    }
                }
            } else if (aepdpk_counters[index].time != 0 && !aepdpk_counters[index].pressed) {
                aepdpk_counters[index].time = 0;
            }
        }
    }
}

static bool aepdpk_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    bool updated_last     = false;
    aepdpk_cooked_changed = false;

    if (aepdpk_counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, aepdpk_last_time);
        aepdpk_last_time          = now;
        updated_last              = true;

        if (elapsed_time > 0) {
            aepdpk_update_and_transfer(raw, cooked, MIN(elapsed_time, 127));
        }
    }

    if (changed || aepdpk_matrix_need_update) {
        if (!updated_last) {
            aepdpk_last_time = timer_read_fast();
        }
        aepdpk_transfer(raw, cooked);
    }

    return aepdpk_cooked_changed;
}

// ---------------------------------------------------------------------------
// Dispatcher
// ---------------------------------------------------------------------------
static const dynamic_debounce_algo_entry_t dynamic_debounce_algorithms[DYNAMIC_DEBOUNCE_ALGO_COUNT] = {
    [DYNAMIC_DEBOUNCE_SYM_DEFER_G]         = {g_init, g_reset, g_debounce, "sym_defer_g"},
    [DYNAMIC_DEBOUNCE_SYM_EAGER_PR]        = {epr_init, epr_reset, epr_debounce, "sym_eager_pr"},
    [DYNAMIC_DEBOUNCE_SYM_DEFER_PR]        = {dpr_init, dpr_reset, dpr_debounce, "sym_defer_pr"},
    [DYNAMIC_DEBOUNCE_SYM_EAGER_PK]        = {epk_init, epk_reset, epk_debounce, "sym_eager_pk"},
    [DYNAMIC_DEBOUNCE_SYM_DEFER_PK]        = {dpk_init, dpk_reset, dpk_debounce, "sym_defer_pk"},
    [DYNAMIC_DEBOUNCE_ASYM_EAGER_DEFER_PK] = {aepdpk_init, aepdpk_reset, aepdpk_debounce, "asym_eager_defer_pk"},
    [DYNAMIC_DEBOUNCE_NONE]                = {none_init, none_reset, none_debounce, "none"},
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
        return none_debounce(raw, cooked, changed);
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

void dynamic_debounce_increase_time(uint8_t step) {
    uint16_t new_time = (uint16_t)dynamic_debounce_time + step;
    dynamic_debounce_set_time(new_time > UINT8_MAX ? UINT8_MAX : (uint8_t)new_time);
}

void dynamic_debounce_decrease_time(uint8_t step) {
    dynamic_debounce_set_time(dynamic_debounce_time > step ? dynamic_debounce_time - step : 0);
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
    return dynamic_debounce_algorithms[algo].name;
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

#ifndef DYNAMIC_DEBOUNCE_TIME_STEP
#    define DYNAMIC_DEBOUNCE_TIME_STEP 1
#endif

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
                dynamic_debounce_increase_time(DYNAMIC_DEBOUNCE_TIME_STEP);
                return false;
            case CM_DYNAMIC_DEBOUNCE_TIME_DOWN:
                dynamic_debounce_decrease_time(DYNAMIC_DEBOUNCE_TIME_STEP);
                return false;
        }
    }
    return true;
}
