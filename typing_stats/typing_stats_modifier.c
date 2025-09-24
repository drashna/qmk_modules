// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_modifier.h"

void ts_mod_record_press(uint8_t mods) {
    if (!ts_core_is_initialized()) return;

    const uint8_t bits[8] = {MOD_BIT(KC_LCTL), MOD_BIT(KC_LSFT), MOD_BIT(KC_LALT), MOD_BIT(KC_LGUI),
                             MOD_BIT(KC_RCTL), MOD_BIT(KC_RSFT), MOD_BIT(KC_RALT), MOD_BIT(KC_RGUI)};

    for (uint8_t i = 0; i < 8; i++) {
        if (mods & bits[i]) {
            ts_core_mod_increment(i); // accessor (no struct pokes)
        }
    }
}

uint32_t ts_mod_get_presses(uint8_t modbit_index) {
    if (!ts_core_is_initialized() || modbit_index >= 8) return 0;
    return ts_core_mod_get_presses(modbit_index);
}

int8_t ts_mod_find_most_used(uint8_t *modbit_out, uint32_t *count_out) {
    if (!ts_core_is_initialized()) return 0;

    uint32_t best_count = 0;
    uint8_t  best_mod   = 0;
    bool     found      = false;

    for (uint8_t i = 0; i < 8; i++) {
        uint32_t presses = ts_core_mod_get_presses(i);
        if (presses > best_count) {
            best_count = presses;
            best_mod   = i;
            found      = true;
        }
    }

    if (found) {
        if (modbit_out) *modbit_out = best_mod;
        if (count_out) *count_out = best_count;
        return 1;
    }
    return 0;
}

int8_t ts_mod_find_least_used(uint8_t *modbit_out, uint32_t *count_out, bool nonzero_only) {
    if (!ts_core_is_initialized()) return 0;

    uint32_t best_count = UINT32_MAX;
    uint8_t  best_mod   = 0;
    bool     found      = false;

    for (uint8_t i = 0; i < 8; i++) {
        uint32_t presses = ts_core_mod_get_presses(i);
        if (nonzero_only && presses == 0) continue;
        if (presses < best_count) {
            best_count = presses;
            best_mod   = i;
            found      = true;
        }
    }

    if (found) {
        if (modbit_out) *modbit_out = best_mod;
        if (count_out) *count_out = (best_count == UINT32_MAX ? 0 : best_count);
        return 1;
    }
    return 0;
}

const char *ts_mod_bit_to_string(uint8_t modbit_index) {
    static const char *mod_names[8] = {"LCtrl", "LShift", "LAlt", "LGui", "RCtrl", "RShift", "RAlt", "RGui"};
    return (modbit_index < 8) ? mod_names[modbit_index] : "Unknown";
}
