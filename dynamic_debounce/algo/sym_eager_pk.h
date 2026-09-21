DEBOUNCE_ALGORITHM(SYM_EAGER_PK)
#ifdef DEBOUNCE_ALGORITHM_IMPLS


// ---------------------------------------------------------------------------
// sym_eager_pk: per-key symmetric eager
// ---------------------------------------------------------------------------
static uint8_t      epk_counters[MATRIX_ROWS_PER_HAND * MATRIX_COLS];
static bool         epk_counters_need_update;
static bool         epk_matrix_need_update;
static bool         epk_cooked_changed;
static fast_timer_t epk_last_time;

static void SYM_EAGER_PK_init(void) {}
static void SYM_EAGER_PK_reset(void) {
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

static bool SYM_EAGER_PK_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

#endif // DEBOUNCE_ALGORITHM_IMPLS
