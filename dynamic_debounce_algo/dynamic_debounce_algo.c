// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "action.h"
#include "action_util.h"
#include "debounce.h"
#include "dynamic_debounce_algo.h"
#include "community_modules.h"
#include "timer.h"
#include "util.h"
#include <string.h>

#ifndef DEBOUNCE
#    define DEBOUNCE 5
#endif

// Maximum debounce for most algorithms
#if DEBOUNCE > UINT8_MAX
#    undef DEBOUNCE
#    define DEBOUNCE UINT8_MAX
#endif

#define DEBOUNCE_ELAPSED 0

static debounce_algorithm_t current_algorithm     = DEBOUNCE_SYM_DEFER_G;
static uint8_t              current_debounce_time = DEBOUNCE;
static bool                 matrix_need_update    = false;
static bool                 counters_need_update  = false;

// Algorithm names for display
static const char *algorithm_names[DEBOUNCE_ALGORITHM_COUNT] = {
    "sym_defer_g", "sym_defer_pk", "sym_defer_pr", "sym_eager_pk", "sym_eager_pr", "asym_eager_defer_pk", "none"};

// ============================================================================
// sym_defer_g implementation
// ============================================================================
static bool debounce_sym_defer_g(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t debouncing_time;
    static bool         debouncing     = false;
    bool                cooked_changed = false;

    if (changed) {
        debouncing      = true;
        debouncing_time = timer_read_fast();
    } else if (debouncing && timer_elapsed_fast(debouncing_time) >= current_debounce_time) {
        size_t matrix_size = MATRIX_ROWS_PER_HAND * sizeof(matrix_row_t);
        if (memcmp(cooked, raw, matrix_size) != 0) {
            memcpy(cooked, raw, matrix_size);
            cooked_changed = true;
        }
        debouncing = false;
    }

    return cooked_changed;
}

// ============================================================================
// sym_defer_pk implementation
// ============================================================================
typedef uint8_t           debounce_counter_t;
static debounce_counter_t defer_pk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS] = {DEBOUNCE_ELAPSED};
static bool               defer_pk_cooked_changed;

static inline void defer_pk_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    counters_need_update = false;
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            if (defer_pk_counters[index] != DEBOUNCE_ELAPSED) {
                if (defer_pk_counters[index] <= elapsed_time) {
                    defer_pk_counters[index] = DEBOUNCE_ELAPSED;
                    matrix_row_t col_mask    = (MATRIX_ROW_SHIFTER << col);
                    matrix_row_t cooked_next = (cooked[row] & ~col_mask) | (raw[row] & col_mask);
                    defer_pk_cooked_changed |= cooked[row] ^ cooked_next;
                    cooked[row] = cooked_next;
                } else {
                    defer_pk_counters[index] -= elapsed_time;
                    counters_need_update = true;
                }
            }
        }
    }
}

static inline void defer_pk_start_counters(matrix_row_t raw[], matrix_row_t cooked[]) {
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset = row * MATRIX_COLS;
        matrix_row_t delta      = raw[row] ^ cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            if (delta & (MATRIX_ROW_SHIFTER << col)) {
                if (defer_pk_counters[index] == DEBOUNCE_ELAPSED) {
                    defer_pk_counters[index] = current_debounce_time;
                    counters_need_update     = true;
                }
            } else {
                defer_pk_counters[index] = DEBOUNCE_ELAPSED;
            }
        }
    }
}

static bool debounce_sym_defer_pk(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t last_time;
    bool                updated_last = false;
    defer_pk_cooked_changed          = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, last_time);

        last_time    = now;
        updated_last = true;

        if (elapsed_time > 0) {
            defer_pk_update_and_transfer(raw, cooked, MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed) {
        if (!updated_last) {
            last_time = timer_read_fast();
        }
        defer_pk_start_counters(raw, cooked);
    }

    return defer_pk_cooked_changed;
}

// ============================================================================
// sym_defer_pr implementation
// ============================================================================
static debounce_counter_t defer_pr_counters[MATRIX_ROWS_PER_HAND] = {DEBOUNCE_ELAPSED};
static bool               defer_pr_cooked_changed;

static inline void defer_pr_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    counters_need_update = false;
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (defer_pr_counters[row] != DEBOUNCE_ELAPSED) {
            if (defer_pr_counters[row] <= elapsed_time) {
                defer_pr_counters[row] = DEBOUNCE_ELAPSED;
                defer_pr_cooked_changed |= cooked[row] ^ raw[row];
                cooked[row] = raw[row];
            } else {
                defer_pr_counters[row] -= elapsed_time;
                counters_need_update = true;
            }
        }
    }
}

static inline void defer_pr_start_counters(matrix_row_t raw[], matrix_row_t cooked[]) {
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (raw[row] != cooked[row]) {
            defer_pr_counters[row] = current_debounce_time;
            counters_need_update   = true;
        } else {
            defer_pr_counters[row] = DEBOUNCE_ELAPSED;
        }
    }
}

static bool debounce_sym_defer_pr(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t last_time;
    bool                updated_last = false;
    defer_pr_cooked_changed          = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, last_time);

        last_time    = now;
        updated_last = true;

        if (elapsed_time > 0) {
            defer_pr_update_and_transfer(raw, cooked, MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed) {
        if (!updated_last) {
            last_time = timer_read_fast();
        }
        defer_pr_start_counters(raw, cooked);
    }

    return defer_pr_cooked_changed;
}

// ============================================================================
// sym_eager_pk implementation
// ============================================================================
static debounce_counter_t eager_pk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS] = {DEBOUNCE_ELAPSED};
static bool               eager_pk_cooked_changed;

static inline void eager_pk_update_counters(uint8_t elapsed_time) {
    counters_need_update = false;
    matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            if (eager_pk_counters[index] != DEBOUNCE_ELAPSED) {
                if (eager_pk_counters[index] <= elapsed_time) {
                    eager_pk_counters[index] = DEBOUNCE_ELAPSED;
                    matrix_need_update       = true;
                } else {
                    eager_pk_counters[index] -= elapsed_time;
                    counters_need_update = true;
                }
            }
        }
    }
}

static inline void eager_pk_transfer_values(matrix_row_t raw[], matrix_row_t cooked[]) {
    eager_pk_cooked_changed = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset   = row * MATRIX_COLS;
        matrix_row_t delta        = raw[row] ^ cooked[row];
        matrix_row_t existing_row = cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            matrix_row_t col_mask = (MATRIX_ROW_SHIFTER << col);
            if (delta & col_mask) {
                if (eager_pk_counters[index] == DEBOUNCE_ELAPSED) {
                    eager_pk_counters[index] = current_debounce_time;
                    counters_need_update     = true;
                    existing_row ^= col_mask; // flip the bit.
                    eager_pk_cooked_changed = true;
                }
            }
        }
        cooked[row] = existing_row;
    }
}

static bool debounce_sym_eager_pk(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t last_time;
    bool                updated_last = false;
    eager_pk_cooked_changed          = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, last_time);

        last_time    = now;
        updated_last = true;

        if (elapsed_time > 0) {
            eager_pk_update_counters(MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed || matrix_need_update) {
        if (!updated_last) {
            last_time = timer_read_fast();
        }
        eager_pk_transfer_values(raw, cooked);
    }

    return eager_pk_cooked_changed;
}

// ============================================================================
// sym_eager_pr implementation
// ============================================================================
static debounce_counter_t eager_pr_counters[MATRIX_ROWS_PER_HAND] = {DEBOUNCE_ELAPSED};
static bool               eager_pr_cooked_changed;

static inline void eager_pr_update_counters(uint8_t elapsed_time) {
    counters_need_update = false;
    matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        if (eager_pr_counters[row] != DEBOUNCE_ELAPSED) {
            if (eager_pr_counters[row] <= elapsed_time) {
                eager_pr_counters[row] = DEBOUNCE_ELAPSED;
                matrix_need_update     = true;
            } else {
                eager_pr_counters[row] -= elapsed_time;
                counters_need_update = true;
            }
        }
    }
}

static inline void eager_pr_transfer_values(matrix_row_t raw[], matrix_row_t cooked[]) {
    eager_pr_cooked_changed = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        matrix_row_t existing_row = cooked[row];
        matrix_row_t raw_row      = raw[row];

        // determine new value basd on debounce pointer + raw value
        if (existing_row != raw_row) {
            if (eager_pr_counters[row] == DEBOUNCE_ELAPSED) {
                eager_pr_counters[row] = current_debounce_time;
                eager_pr_cooked_changed |= cooked[row] ^ raw_row;
                cooked[row]          = raw_row;
                counters_need_update = true;
            }
        }
    }
}

static bool debounce_sym_eager_pr(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t last_time;
    bool                updated_last = false;
    eager_pr_cooked_changed          = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, last_time);

        last_time    = now;
        updated_last = true;

        if (elapsed_time > 0) {
            eager_pr_update_counters(MIN(elapsed_time, UINT8_MAX));
        }
    }

    if (changed || matrix_need_update) {
        if (!updated_last) {
            last_time = timer_read_fast();
        }
        eager_pr_transfer_values(raw, cooked);
    }

    return eager_pr_cooked_changed;
}

// ============================================================================
// asym_eager_defer_pk implementation (max debounce 127ms)
// ============================================================================
typedef struct {
    bool    pressed : 1;
    uint8_t time    : 7;
} asym_debounce_counter_t;

static asym_debounce_counter_t asym_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS] = {0};
static bool                    asym_cooked_changed;

static inline void asym_update_and_transfer(matrix_row_t raw[], matrix_row_t cooked[], uint8_t elapsed_time) {
    counters_need_update = false;
    matrix_need_update   = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t row_offset = row * MATRIX_COLS;

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t index = row_offset + col;

            if (asym_counters[index].time != DEBOUNCE_ELAPSED) {
                if (asym_counters[index].time <= elapsed_time) {
                    asym_counters[index].time = DEBOUNCE_ELAPSED;

                    if (asym_counters[index].pressed) {
                        // key-down: eager
                        matrix_need_update = true;
                    } else {
                        // key-up: defer
                        matrix_row_t col_mask    = (MATRIX_ROW_SHIFTER << col);
                        matrix_row_t cooked_next = (cooked[row] & ~col_mask) | (raw[row] & col_mask);
                        asym_cooked_changed |= cooked_next ^ cooked[row];
                        cooked[row] = cooked_next;
                    }
                } else {
                    asym_counters[index].time -= elapsed_time;
                    counters_need_update = true;
                }
            }
        }
    }
}

static inline void asym_transfer_values(matrix_row_t raw[], matrix_row_t cooked[]) {
    uint8_t debounce_clamped = MIN(current_debounce_time, 127);
    counters_need_update     = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
        uint16_t     row_offset = row * MATRIX_COLS;
        matrix_row_t delta      = raw[row] ^ cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t     index    = row_offset + col;
            matrix_row_t col_mask = (MATRIX_ROW_SHIFTER << col);

            if (delta & col_mask) {
                if (asym_counters[index].time == DEBOUNCE_ELAPSED) {
                    asym_counters[index].pressed = (raw[row] & col_mask);
                    asym_counters[index].time    = debounce_clamped;
                    counters_need_update         = true;

                    if (asym_counters[index].pressed) {
                        // key-down: eager
                        cooked[row] ^= col_mask;
                        asym_cooked_changed = true;
                    }
                }
            } else if (asym_counters[index].time != DEBOUNCE_ELAPSED) {
                if (!asym_counters[index].pressed) {
                    // key-up: defer
                    asym_counters[index].time = DEBOUNCE_ELAPSED;
                }
            }
        }
    }
}

static bool debounce_asym_eager_defer_pk(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    static fast_timer_t last_time;
    bool                updated_last = false;
    asym_cooked_changed              = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_time = TIMER_DIFF_FAST(now, last_time);

        last_time    = now;
        updated_last = true;

        if (elapsed_time > 0) {
            asym_update_and_transfer(raw, cooked, MIN(elapsed_time, 127));
        }
    }

    if (changed || matrix_need_update) {
        if (!updated_last) {
            last_time = timer_read_fast();
        }
        asym_transfer_values(raw, cooked);
    }

    return asym_cooked_changed;
}

// ============================================================================
// None implementation
// ============================================================================
static bool debounce_none(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

// ============================================================================
// Main API implementation
// ============================================================================

void debounce_init(void) {
    memset(defer_pk_counters, DEBOUNCE_ELAPSED, sizeof(defer_pk_counters));
    memset(defer_pr_counters, DEBOUNCE_ELAPSED, sizeof(defer_pr_counters));
    memset(eager_pk_counters, DEBOUNCE_ELAPSED, sizeof(eager_pk_counters));
    memset(eager_pr_counters, DEBOUNCE_ELAPSED, sizeof(eager_pr_counters));
    memset(asym_counters, 0, sizeof(asym_counters));
    counters_need_update = false;
    matrix_need_update   = false;
}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    if (current_debounce_time == 0) {
        return debounce_none(raw, cooked, changed);
    }
    switch (current_algorithm) {
        case DEBOUNCE_SYM_DEFER_G:
            return debounce_sym_defer_g(raw, cooked, changed);
        case DEBOUNCE_SYM_DEFER_PK:
            return debounce_sym_defer_pk(raw, cooked, changed);
        case DEBOUNCE_SYM_DEFER_PR:
            return debounce_sym_defer_pr(raw, cooked, changed);
        case DEBOUNCE_SYM_EAGER_PK:
            return debounce_sym_eager_pk(raw, cooked, changed);
        case DEBOUNCE_SYM_EAGER_PR:
            return debounce_sym_eager_pr(raw, cooked, changed);
        case DEBOUNCE_ASYM_EAGER_DEFER_PK:
            return debounce_asym_eager_defer_pk(raw, cooked, changed);
        case DEBOUNCE_NONE:
            return debounce_none(raw, cooked, changed);
        default:
            return debounce_sym_defer_g(raw, cooked, changed);
    }
}

__attribute__((weak)) void debounce_set_algorithm_user(debounce_algorithm_t algorithm) {}

__attribute__((weak)) void debounce_set_algorithm_kb(debounce_algorithm_t algorithm) {
    debounce_set_algorithm_user(algorithm);
}

bool debounce_set_algorithm_impl(debounce_algorithm_t algorithm) {
    bool has_changed = false;
    if (algorithm >= DEBOUNCE_ALGORITHM_COUNT) {
        algorithm = DEBOUNCE_NONE;
    }

    if (current_algorithm != algorithm) {
        current_algorithm = algorithm;
        has_changed       = true;
    }

    return has_changed;
}

void debounce_set_algorithm(debounce_algorithm_t algorithm) {
    if (debounce_set_algorithm_impl(algorithm)) {
        debounce_init();
        clear_keyboard();
        debounce_set_algorithm_kb(algorithm);
    }
}

debounce_algorithm_t debounce_get_algorithm(void) {
    return current_algorithm;
}

const char *debounce_get_algorithm_name(debounce_algorithm_t algorithm) {
    if (algorithm < DEBOUNCE_ALGORITHM_COUNT) {
        return algorithm_names[algorithm];
    }
    return "unknown";
}

__attribute__((weak)) void debounce_set_time_user(uint8_t time_ms) {}

__attribute__((weak)) void debounce_set_time_kb(uint8_t time_ms) {
    debounce_set_time_user(time_ms);
}

void debounce_set_time_impl(uint8_t time_ms) {
    // Clamp to safe values based on current algorithm
    if (current_algorithm == DEBOUNCE_ASYM_EAGER_DEFER_PK) {
        current_debounce_time = MIN(time_ms, 127);
    } else {
        current_debounce_time = time_ms;
    }
}

void debounce_set_time(uint8_t time_ms) {
    // Clamp to safe values based on current algorithm
    debounce_set_time_impl(time_ms);
    // Reset all counters when debounce time changes
    debounce_init();
    clear_keyboard();
    debounce_set_time_kb(time_ms);
}

uint8_t debounce_get_time(void) {
    return current_debounce_time;
}

void debounce_set_algorithm_and_time(debounce_algorithm_t algorithm, uint8_t time_ms) {
    debounce_set_algorithm_impl(algorithm);
    debounce_set_time_impl(time_ms);
    debounce_init();
    if (is_keyboard_master()) {
        clear_keyboard();
    }
    debounce_set_time_kb(time_ms);
    debounce_set_algorithm_kb(algorithm);
}

#if 0
// For whatever reason, syncing this over split here causes issues with both sides becoming slow/stuck.
#    ifdef SPLIT_KEYBOARD
#        include "transactions.h"

// Split keyboard synchronization
typedef struct {
    debounce_algorithm_t algorithm;
    uint8_t              time_ms;
} debounce_config_t;

static void debounce_config_slave_handler(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer,
                                          uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    const debounce_config_t *config = (const debounce_config_t *)initiator2target_buffer;
    if (config->algorithm >= DEBOUNCE_ALGORITHM_COUNT) {
        return;
    }
    if (config->algorithm != current_algorithm || config->time_ms != current_debounce_time) {
        debounce_set_algorithm_and_time(config->algorithm, config->time_ms);
    }
}

void keyboard_post_init_dynamic_debounce_algo(void) {
    transaction_register_rpc(RPC_ID_DYNAMIC_DEBOUNCE_ALGO_SYNC, debounce_config_slave_handler);
    keyboard_post_init_dynamic_debounce_algo_kb();
}

void housekeeping_task_dynamic_debounce_algo(void) {
    static debounce_algorithm_t last_synced_algorithm = DEBOUNCE_SYM_DEFER_G;
    static uint8_t              last_synced_time      = DEBOUNCE;

    if (current_algorithm != last_synced_algorithm || current_debounce_time != last_synced_time) {
        debounce_config_t config = {.algorithm = current_algorithm, .time_ms = current_debounce_time};

        if (transaction_rpc_send(RPC_ID_DYNAMIC_DEBOUNCE_ALGO_SYNC, sizeof(config), &config)) {
            last_synced_algorithm = current_algorithm;
            last_synced_time      = current_debounce_time;
        }
    }
    housekeeping_task_dynamic_debounce_algo_kb();
}
#    endif
#endif

// ============================================================================
// Keycode handling
// ============================================================================

bool process_record_dynamic_debounce_algo(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_dynamic_debounce_algo_kb(keycode, record)) {
        return false;
    }
    switch (keycode) {
        case DYNAMIC_DEBOUNCE_ALGO_CYCLE:
            if (record->event.pressed) {
                debounce_algorithm_t next_algo;
                if (get_mods() & MOD_MASK_SHIFT) {
                    // Cycle backwards
                    next_algo = (current_algorithm == 0) ? (DEBOUNCE_ALGORITHM_COUNT - 1) : (current_algorithm - 1);
                } else {
                    // Cycle forwards
                    next_algo = (current_algorithm + 1) % DEBOUNCE_ALGORITHM_COUNT;
                }
                debounce_set_algorithm(next_algo);
            }
            return true;
        case DYNAMIC_DEBOUNCE_TIME_UP:
            if (record->event.pressed) {
                uint8_t max_time  = (current_algorithm == DEBOUNCE_ASYM_EAGER_DEFER_PK) ? 127 : UINT8_MAX;
                uint8_t increment = (get_mods() & MOD_MASK_SHIFT) ? 10 : 1;
                uint8_t new_time  = MIN(current_debounce_time + increment, max_time);
                debounce_set_time(new_time);
            }
            return true;
        case DYNAMIC_DEBOUNCE_TIME_DOWN:
            if (record->event.pressed) {
                uint8_t decrement = (get_mods() & MOD_MASK_SHIFT) ? 10 : 1;
                if (current_debounce_time >= decrement) {
                    debounce_set_time(current_debounce_time - decrement);
                } else {
                    debounce_set_time(0);
                }
            }
            return true;
    }
    return true;
}
