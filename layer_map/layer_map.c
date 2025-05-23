// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "quantum.h"
#include "layer_map.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

uint16_t    layer_map[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {0};
static bool layer_map_set                             = true;
extern bool peek_matrix(uint8_t row_index, uint8_t col_index, bool read_raw);
static bool layer_map_has_updated = false;
#if defined(SWAP_HANDS_ENABLE) && defined(ENCODER_MAP_ENABLE)
#    include "encoder.h"
extern const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS];
#endif // SWAP_HANDS_ENABLE && ENCODER_MAP_ENABLE

void set_layer_map_dirty(void) {
    layer_map_set = true;
}

bool get_layer_map_has_updated(void) {
    return layer_map_has_updated;
}

void set_layer_map_has_updated(bool value) {
    layer_map_has_updated = value;
}

void populate_layer_map(void) {
    for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
        for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
#ifdef LAYER_MAP_REMAPPING
            keypos_t key = layer_remap[i][j];
#else  // LAYER_MAP_REMAPPING
            keypos_t key = {.row = i, .col = j};
#endif // LAYER_MAP_REMAPPING

#ifdef SWAP_HANDS_ENABLE
            if (is_swap_hands_on()) {
                if (key.row < MATRIX_ROWS && key.col < MATRIX_COLS) {
#    ifdef LAYER_MAP_REMAPPING
                    key.row = pgm_read_byte(&hand_swap_config[layer_remap[i][j].row][layer_remap[i][j].col].row);
                    key.col = pgm_read_byte(&hand_swap_config[layer_remap[i][j].row][layer_remap[i][j].col].col);
#    else  // LAYER_MAP_REMAPPING
                    key.row = pgm_read_byte(&hand_swap_config[i][j].row);
                    key.col = pgm_read_byte(&hand_swap_config[i][j].col);
#    endif // LAYER_MAP_REMAPPING
                } else if (key.row == KEYLOC_ENCODER_CCW || key.row == KEYLOC_ENCODER_CW) {
                    key.col = pgm_read_byte(&encoder_hand_swap_config[key.col]);
                }
            }
#endif // SWAP_HANDS_ENABLE
            layer_map[i][j] = keymap_key_to_keycode(layer_switch_get_layer(key), key);
        }
    }
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    layer_map_has_updated = true;
#endif
}

bool peek_matrix_layer_map(uint8_t row, uint8_t col) {
#ifdef LAYER_MAP_REMAPPING
    if (layer_remap[row][col].row >= KEYLOC_DIP_SWITCH_OFF) {
        return false;
    }
    return peek_matrix(layer_remap[row][col].row, layer_remap[row][col].col, false);
#else  // LAYER_MAP_REMAPPING
    if (row >= KEYLOC_DIP_SWITCH_OFF) {
        return false;
    }
    return peek_matrix(row, col, false);
#endif // LAYER_MAP_REMAPPING
}

__attribute__((weak)) layer_state_t layer_state_set_layer_map_user(layer_state_t state) {
    return state;
}

__attribute__((weak)) layer_state_t layer_state_set_layer_map_kb(layer_state_t state) {
    return layer_state_set_layer_map_user(state);
}

layer_state_t layer_state_set_layer_map(layer_state_t state) {
    if ((COMMUNITY_MODULES_API_VERSION) >= COMMUNITY_MODULES_API_VERSION_BUILDER(1, 1, 0)) {
        state         = layer_state_set_layer_map_kb(state);
        layer_map_set = true;
    }

    return state;
}

__attribute__((weak)) layer_state_t default_layer_state_set_layer_map_user(layer_state_t state) {
    return state;
}

__attribute__((weak)) layer_state_t default_layer_state_set_layer_map_kb(layer_state_t state) {
    return layer_state_set_layer_map_user(state);
}

layer_state_t default_layer_state_set_layer_map(layer_state_t state) {
    if ((COMMUNITY_MODULES_API_VERSION) >= COMMUNITY_MODULES_API_VERSION_BUILDER(1, 1, 0)) {
        state         = default_layer_state_set_layer_map_kb(state);
        layer_map_set = true;
    }

    return state;
}

void housekeeping_task_layer_map(void) {
    if ((COMMUNITY_MODULES_API_VERSION) < COMMUNITY_MODULES_API_VERSION_BUILDER(1, 1, 0)) {
        static layer_state_t last_layer_state = 0, last_default_layer_state = 0;
        if (layer_state != last_layer_state || default_layer_state != last_default_layer_state) {
            last_layer_state         = layer_state;
            last_default_layer_state = default_layer_state;
            layer_map_set            = true;
        }
    }

#ifdef SWAP_HANDS_ENABLE
    static bool swap_hands = false;
    if (is_swap_hands_on() != swap_hands) {
        swap_hands    = is_swap_hands_on();
        layer_map_set = true;
    }
#endif // SWAP_HANDS_ENABLE
    if (layer_map_set) {
        if (is_keyboard_master()) {
            populate_layer_map();
        }
        layer_map_set = false;
    }
    housekeeping_task_layer_map_kb();
}

#ifdef VIA_ENABLE
#    include "via.h"

__attribute__((weak)) bool via_command_kb(uint8_t *data, uint8_t length) {
    switch (data[0]) {
        case id_dynamic_keymap_set_keycode:
        case id_dynamic_keymap_reset:
        case id_dynamic_keymap_set_buffer:
        case id_dynamic_keymap_set_encoder:
            set_layer_map_dirty();
            break;
    }
    return false;
}
#endif // VIA_ENABLE
