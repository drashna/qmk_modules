DEBOUNCE_ALGORITHM(SYM_DEFER_PR)
#ifdef DEBOUNCE_ALGORITHM_IMPLS


// ---------------------------------------------------------------------------
// sym_defer_pr: per-row symmetric defer
// ---------------------------------------------------------------------------
static uint8_t      dpr_counters[MATRIX_ROWS_PER_HAND];
static bool         dpr_counters_need_update;
static bool         dpr_cooked_changed;
static fast_timer_t dpr_last_time;

static void SYM_DEFER_PR_init(void) {}
static void SYM_DEFER_PR_reset(void) {
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

static bool SYM_DEFER_PR_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

#endif // DEBOUNCE_ALGORITHM_IMPLS
