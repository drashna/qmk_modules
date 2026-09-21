DEBOUNCE_ALGORITHM(SYM_EAGER_PR)
#ifdef DEBOUNCE_ALGORITHM_IMPLS

// ---------------------------------------------------------------------------
// sym_eager_pr: per-row symmetric eager
// ---------------------------------------------------------------------------
static uint8_t      epr_counters[MATRIX_ROWS_PER_HAND];
static bool         epr_counters_need_update;
static bool         epr_matrix_need_update;
static bool         epr_cooked_changed;
static fast_timer_t epr_last_time;

static void SYM_EAGER_PR_init(void) {}
static void SYM_EAGER_PR_reset(void) {
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

static bool SYM_EAGER_PR_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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
#endif // DEBOUNCE_ALGORITHM_IMPLS
