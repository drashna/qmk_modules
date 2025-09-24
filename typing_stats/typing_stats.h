// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "typing_stats_analysis.h"
#include "typing_stats_bigram.h"
#include "typing_stats_core.h"
#include "typing_stats_layer.h"
#include "typing_stats_modifier.h"
#include "typing_stats_position.h"
#include "typing_stats_storage.h"

void          ts_init(void);
void          ts_task_10ms(void);
void          ts_on_keyevent(keyrecord_t *record, uint16_t keycode);
layer_state_t ts_on_layer_change(layer_state_t new_state);
void          ts_eeconfig_init_user(void);
