DEBOUNCE_ALGORITHM(SYM_DEFER_PK)
#ifdef DEBOUNCE_ALGORITHM_IMPLS


// ---------------------------------------------------------------------------
// sym_defer_pk: per-key symmetric defer
// ---------------------------------------------------------------------------
static uint8_t      dpk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS];
static bool         dpk_counters_need_update;
static bool         dpk_cooked_changed;
static fast_timer_t dpk_last_time;

static void SYM_DEFER_PK_init(void) {}
static void SYM_DEFER_PK_reset(void) {
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

static bool SYM_DEFER_PK_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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
#endif // DEBOUNCE_ALGORITHM_IMPLS
