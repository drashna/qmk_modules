// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>
#include "keyboard.h"

#ifndef LAYER_MAP_ROWS
#    define LAYER_MAP_ROWS MATRIX_ROWS
#endif // !LAYER_MAP_ROWS
#ifndef LAYER_MAP_COLS
#    define LAYER_MAP_COLS MATRIX_COLS
#endif // !LAYER_MAP_COLS

extern volatile uint16_t layer_map[LAYER_MAP_ROWS][LAYER_MAP_COLS];
void                     set_layer_map_dirty(void);
bool                     get_layer_map_has_updated(void);
void                     set_layer_map_has_updated(bool value);
bool                     peek_matrix_layer_map(uint8_t row, uint8_t col);

#ifdef LAYER_MAP_REMAPPING
extern keypos_t layer_remap[LAYER_MAP_ROWS][LAYER_MAP_COLS];
#endif // LAYER_MAP_REMAPPING
