// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef QUANTUM_PAINTER_ENABLE
#    include <qp.h>
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
void qp_triangle(painter_device_t device, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3,
                 uint8_t hue, uint8_t sat, uint8_t val, bool filled);
void qp_rounded_rect(painter_device_t device, int16_t left, int16_t top, int16_t right, int16_t bottom, int16_t radius,
                     uint8_t hue, uint8_t sat, uint8_t val, bool filled);
void draw_vertical_scale(painter_device_t device, int x, int y, int height, int spacing, int divisions);
void draw_horizontal_scale(painter_device_t device, int x, int y, int width, int spacing, int divisions);
void draw_line_graph_impl(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count,
                          bool line, bool fill);
void draw_line_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count,
                     bool line, bool fill);
void draw_area_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count);
void draw_dot_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count);
void draw_bar_graph_impl(painter_device_t device, int x, int y, int width, int height, int data[], int data_count,
                         int bar_width, int bar_padding);
void draw_bar_graph(painter_device_t device, int x, int y, int width, int height, int data[], int data_count);
void draw_autoscale_bar_graph(painter_device_t device, int x, int y, int width, int height, int data[], int data_count);
void draw_linear_gauge(painter_device_t device, int x, int y, int width, int height, int value);
void draw_needle_meter(painter_device_t device, int x, int y, int width, int value);
void draw_signal_strength(painter_device_t device, int x, int y, int width, int height, int value);
void draw_thermometer(painter_device_t device, int x, int y, int width, int height, int value);
void draw_segmented_gauge(painter_device_t device, int x, int y, int width, int height, int value, int segments);
void draw_dot_gauge(painter_device_t device, int x, int y, int width, int height, int value);
void draw_radial_gauge(painter_device_t device, int x, int y, int radius, int value, int padding);
void draw_radial_dot_gauge(painter_device_t device, int x, int y, int radius, int dot_radius, int value);
void draw_radial_segment_gauge(painter_device_t device, int x, int y, int radius, int segments, int value);
void draw_radial_line_gauge(painter_device_t device, int x, int y, int radius, int lines, int value);
#endif
