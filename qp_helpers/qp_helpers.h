// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef QUANTUM_PAINTER_ENABLE
#    include <qp.h>
#    include <stdlib.h>
#    include <quantum/color.h>

typedef enum drawing_mode_t {
    LINE,
    POINT,
    DOT,
    SQUARED_LINE,
} drawing_mode_t;

typedef struct {
    uint8_t       *data;
    hsv_t          color;
    drawing_mode_t mode;
    uint8_t        max_value;
} graph_line_t;

#    define GRAPHS_END   \
        (graph_line_t) { \
            .data = NULL \
        }

typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

typedef struct {
    painter_device_t device;
    point_t          start;
    point_t          size;
    hsv_t            axis;
    hsv_t            background;
    uint8_t          data_points;
} graph_config_t;

bool qp_draw_graph(const graph_config_t *config, const graph_line_t *lines);
#endif
