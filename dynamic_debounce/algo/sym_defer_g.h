DEBOUNCE_ALGORITHM(SYM_DEFER_G)
#ifdef DEBOUNCE_ALGORITHM_IMPLS

// ---------------------------------------------------------------------------
// sym_defer_g: global symmetric defer
// ---------------------------------------------------------------------------
static fast_timer_t g_debouncing_time;
static bool         g_debouncing;

static void SYM_DEFER_G_init(void) {}
static void SYM_DEFER_G_reset(void) {
    g_debouncing = false;
}

static bool SYM_DEFER_G_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

#endif // DEBOUNCE_ALGORITHM_IMPLS
