// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>

enum unicode_typing_modes {
    UCTM_NO_MODE,
    UCTM_WIDE,
    UCTM_SCRIPT,
    UCTM_BLOCKS,
    UCTM_REGIONAL,
    UCTM_AUSSIE,
    UCTM_ZALGO,
    UCTM_SUPER,
    UCTM_COMIC,
    UCTM_FRAKTUR,
    UCTM_DOUBLE_STRUCK,
    UCTM_SCREAM_CYPHER,
    UNCODES_MODE_END,
};

void set_unicode_typing_mode(uint8_t mode);
uint8_t get_unicode_typing_mode(void);
const char *get_unicode_typing_mode_str(uint8_t mode);
