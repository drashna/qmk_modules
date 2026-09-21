DEBOUNCE_ALGORITHM(ASYM_EAGER_DEFER_PK)
#ifdef DEBOUNCE_ALGORITHM_IMPLS

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

static void ASYM_EAGER_DEFER_PK_init(void) {}
static void ASYM_EAGER_DEFER_PK_reset(void) {
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

static bool ASYM_EAGER_DEFER_PK_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

#endif //
