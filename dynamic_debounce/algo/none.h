DEBOUNCE_ALGORITHM(NONE)
#ifdef DEBOUNCE_ALGORITHM_IMPLS

// ---------------------------------------------------------------------------
// none: passthrough, no debouncing at all
// ---------------------------------------------------------------------------
static void NONE_init(void) {}
static void NONE_reset(void) {}
static bool NONE_debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
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

#endif
