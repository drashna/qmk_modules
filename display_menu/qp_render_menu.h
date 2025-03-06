// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "quantum/color.h"
#include "qp.h"

bool painter_render_menu(painter_device_t display, painter_font_handle_t font, uint16_t start_x, uint16_t start_y,
                         uint16_t end_x, uint16_t end_y, bool is_verbose, hsv_t primary, hsv_t secondary);
