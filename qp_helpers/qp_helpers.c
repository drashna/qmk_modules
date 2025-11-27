// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp_helpers.h"
#    include <math.h>
#    include <stdint.h>

// PI constant for trigonometric functions
#    ifndef M_PI
#        define M_PI 3.14159265358979323846
#    endif
#    define PI M_PI

typedef bool (*draw_fn_t)(const graph_config_t *config, const graph_line_t *line);

//

static inline uint8_t scale_value(uint8_t value, uint8_t from, uint8_t to) {
    return (MIN(value, to) * from / to);
}

static inline uint16_t in_range(uint16_t value, uint16_t start, uint16_t end) {
    return MIN(MAX(start, value), end);
}

static point_t get_end(const graph_config_t *config) {
    return (point_t){
        .x = config->start.x + config->size.x,
        .y = config->start.y + config->size.y,
    };
}

static bool draw_line(const graph_config_t *config, const graph_line_t *line) {
    const uint8_t step      = config->size.x / config->data_points;
    const uint8_t remainder = config->size.x - (config->data_points * step);

    const point_t end    = get_end(config);
    uint8_t       offset = 0;

    // -1 because we will also access next point on each iteration
    for (uint8_t n = 0; n < config->data_points - 1; ++n) {
        offset += (remainder != 0 && n % (config->data_points / remainder) == 0) ? 1 : 0;

        const uint16_t x1 = MIN(config->start.x + (step * n) + offset, end.x);
        const uint16_t y1 = end.y - scale_value(line->data[n], config->size.y - 1, line->max_value);

        const uint16_t x2 = MIN(config->start.x + (step * (n + 1)) + offset, end.x);
        const uint16_t y2 = end.y - scale_value(line->data[n + 1], config->size.y - 1, line->max_value);

        if (!qp_line(config->device, x1, y1, x2, y2, line->color.h, line->color.s, line->color.v)) {
            return false;
        }
    }

    return true;
}

static bool draw_point(const graph_config_t *config, const graph_line_t *line) {
    const uint16_t step      = config->size.x / config->data_points;
    const uint8_t  remainder = config->size.x - (config->data_points * step);

    const point_t end    = get_end(config);
    uint8_t       offset = 0;

    for (uint8_t n = 0; n < config->data_points; ++n) {
        offset += (remainder != 0 && n % (config->data_points / remainder) == 0) ? 1 : 0;
        const uint16_t x = MIN(config->start.x + (step * n) + offset, end.x);
        const uint16_t y = end.y - scale_value(line->data[n], config->size.y - 1, line->max_value);

        if (!qp_setpixel(config->device, x, y, line->color.h, line->color.s, line->color.v)) {
            return false;
        }
    }

    return true;
}

static bool draw_dot(const graph_config_t *config, const graph_line_t *line) {
    const uint16_t step      = config->size.x / config->data_points;
    const uint8_t  remainder = config->size.x - (config->data_points * step);

    const point_t end    = get_end(config);
    uint8_t       offset = 1;

    for (uint8_t n = 0; n < config->data_points; ++n) {
        offset += (remainder != 0 && n % (config->data_points / remainder) == 0) ? 1 : 0;
        const uint16_t x = MIN(config->start.x + (step * n) + offset, end.x);
        const uint16_t y = end.y - scale_value(line->data[n], config->size.y - 1, line->max_value);

        if (!qp_rect(
                config->device, x - 1, y - ((y < config->size.y) ? 1 : 0), MIN(x + 1, end.x - 1),
                y + (y >= (config->start.y + config->size.y - 1) ? ((y > (config->start.y + config->size.y)) ? -1 : 0)
                                                                 : 1),
                line->color.h, line->color.s, line->color.v, true)) {
            return false;
        }
    }

    return true;
}

static bool draw_square_line(const graph_config_t *config, const graph_line_t *line) {
    const uint16_t step      = config->size.x / config->data_points;
    const uint8_t  remainder = config->size.x - (config->data_points * step);

    const point_t end    = get_end(config);
    uint8_t       offset = 0;

    // -1 because we will also access next point on each iteration
    for (uint8_t n = 0; n < config->data_points - 1; ++n) {
        offset += (remainder != 0 && n % (config->data_points / remainder) == 0) ? 1 : 0;
        const uint16_t x1 = MIN(config->start.x + (step * n) + offset, end.x);
        uint16_t       y1 = end.y - scale_value(line->data[n], config->size.y - 1, line->max_value);

        const uint16_t x2 = MIN(config->start.x + (step * (n + 1)) + offset, end.x);
        const uint16_t y2 = end.y - scale_value(line->data[n + 1], config->size.y - 1, line->max_value);

        if (!qp_line(config->device, x1, y1, x2, y1, line->color.h, line->color.s, line->color.v)) {
            return false;
        }

        if (!qp_line(config->device, x2, y1, x2, y2, line->color.h, line->color.s, line->color.v)) {
            return false;
        }
    }

    return true;
}

static const draw_fn_t draw_functions[] = {
    [LINE]         = draw_line,
    [POINT]        = draw_point,
    [DOT]          = draw_dot,
    [SQUARED_LINE] = draw_square_line,
};

bool qp_draw_graph(const graph_config_t *config, const graph_line_t *lines) {
    // if there are more segments than the graph width is wide in pixels, reject drawing
    if (config->data_points >= config->size.x) {
        return false;
    }

    // clear the graph area for redrawing
    if (!qp_rect(config->device, config->start.x, config->start.y, config->start.x + config->size.x,
                 config->start.y + config->size.y - 1, config->background.h, config->background.s, config->background.v,
                 true)) {
        return false;
    }

    // Draw graph axes
    if (!qp_line(config->device, config->start.x, config->start.y, config->start.x,
                 config->start.y + config->size.y - 1, config->axis.h, config->axis.s, config->axis.v)) {
        return false;
    }

    if (!qp_line(config->device, config->start.x, (config->start.y + config->size.y - 1),
                 config->start.x + config->size.x, (config->start.y + config->size.y) - 1, config->axis.h,
                 config->axis.s, config->axis.v)) {
        return false;
    }

    const graph_line_t *line = lines;
    while (line->data != NULL) {
        const draw_fn_t function = draw_functions[line->mode];
        if (!function(config, line)) {
            return false;
        }

        line++;
    }

    return true;
}

// Helper function to draw a triangle using lines
void qp_triangle(painter_device_t device, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3,
                 uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    if (filled) {
        // Filled triangle using scanline algorithm
        // Sort vertices by y coordinate
        if (y1 > y2) {
            int tmp = x1;
            x1      = x2;
            x2      = tmp;
            tmp     = y1;
            y1      = y2;
            y2      = tmp;
        }
        if (y2 > y3) {
            int tmp = x2;
            x2      = x3;
            x3      = tmp;
            tmp     = y2;
            y2      = y3;
            y3      = tmp;
        }
        if (y1 > y2) {
            int tmp = x1;
            x1      = x2;
            x2      = tmp;
            tmp     = y1;
            y1      = y2;
            y2      = tmp;
        }

        // Draw filled triangle
        float dx1 = abs(x2 - x1), dy1 = abs(y2 - y1);
        float dx2 = abs(x3 - x1), dy2 = abs(y3 - y1);

        float signx1 = (x2 > x1) ? 1 : -1;
        float signx2 = (x3 > x1) ? 1 : -1;
        float signy1 = (y2 > y1) ? 1 : -1;
        // float signy2 = (y3 > y1) ? 1 : -1;

        if (dy1 > dx1) {
            int tmp = dx1;
            dx1     = dy1;
            dy1     = tmp;
        }
        if (dy2 > dx2) {
            int tmp = dx2;
            dx2     = dy2;
            dy2     = tmp;
        }

        float e1 = 2 * dy1 - dx1;
        float e2 = 2 * dy2 - dx2;

        float x1_f = x1, x2_f = x1;

        for (int i = 0; i <= (int)dx1; i++) {
            int y_coord = (int)(y1 + i * signy1);
            qp_line(device, (int)x1_f, y_coord, (int)x2_f, y_coord, hue, sat, val);

            if (e1 < 0) {
                e1 += 2 * dy1;
            } else {
                e1 += 2 * (dy1 - dx1);
                x1_f += signx1;
            }
            if (e2 < 0) {
                e2 += 2 * dy2;
            } else {
                e2 += 2 * (dy2 - dx2);
                x2_f += signx2;
            }
        }
    } else {
        // Outline only
        qp_line(device, x1, y1, x2, y2, hue, sat, val);
        qp_line(device, x2, y2, x3, y3, hue, sat, val);
        qp_line(device, x3, y3, x1, y1, hue, sat, val);
    }
}

// Helper function to draw a rounded rectangle
void qp_rounded_rect(painter_device_t device, int16_t left, int16_t top, int16_t right, int16_t bottom, int16_t radius,
                     uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    if (filled) {
        // Draw filled rounded rectangle
        int16_t r = radius;

        // Draw main rectangle
        qp_rect(device, left + r, top, right - r, bottom, hue, sat, val, true);
        qp_rect(device, left, top + r, right, bottom - r, hue, sat, val, true);

        // Draw corner circles (as quarter circles)
        for (int16_t x = 0; x <= r; x++) {
            int16_t y = (int16_t)(r - sqrt(r * r - x * x));
            // Top-left
            qp_line(device, left + r - x, top + r - y, left + r - x, top + r + y, hue, sat, val);
            // Top-right
            qp_line(device, right - r + x, top + r - y, right - r + x, top + r + y, hue, sat, val);
            // Bottom-left
            qp_line(device, left + r - x, bottom - r - y, left + r - x, bottom - r + y, hue, sat, val);
            // Bottom-right
            qp_line(device, right - r + x, bottom - r - y, right - r + x, bottom - r + y, hue, sat, val);
        }
    } else {
        // Draw outlined rounded rectangle
        qp_line(device, left + radius, top, right - radius, top, hue, sat, val);
        qp_line(device, left + radius, bottom, right - radius, bottom, hue, sat, val);
        qp_line(device, left, top + radius, left, bottom - radius, hue, sat, val);
        qp_line(device, right, top + radius, right, bottom - radius, hue, sat, val);

        // Corner arcs (approximated with lines)
        for (int16_t x = 0; x <= radius; x++) {
            int16_t y = (int16_t)(radius - sqrt(radius * radius - x * x));
            qp_line(device, left + radius - x, top + radius - y, left + radius - x, top + radius - y, hue, sat, val);
            qp_line(device, right - radius + x, top + radius - y, right - radius + x, top + radius - y, hue, sat, val);
            qp_line(device, left + radius - x, bottom - radius + y, left + radius - x, bottom - radius + y, hue, sat,
                    val);
            qp_line(device, right - radius + x, bottom - radius + y, right - radius + x, bottom - radius + y, hue, sat,
                    val);
        }
    }
}

// ============================================================================
// SHARED FUNCTIONS
// ============================================================================

void draw_vertical_scale(painter_device_t device, int x, int y, int height, int spacing, int divisions) {
    int bottom = y + height - 1;

    for (int i = 0; i <= height / spacing; i++) {
        int tick_y = bottom - i * spacing;
        qp_line(device, x, tick_y, x, tick_y, HSV_WHITE);
        if (i % divisions == 0) {
            qp_line(device, x + 1, tick_y, x + 1, tick_y, HSV_WHITE);
        }
    }
}

void draw_horizontal_scale(painter_device_t device, int x, int y, int width, int spacing, int divisions) {
    for (int i = 0; i < width / spacing; i++) {
        int tick_x = x + i * spacing;
        qp_line(device, tick_x, y, tick_x, y, HSV_WHITE);
        if (i % divisions == 0) {
            qp_line(device, tick_x, y - 1, tick_x, y - 1, HSV_WHITE);
        }
    }
}

// ============================================================================
// GRAPHS
// ============================================================================

/*
 * Line Graph
 */
void draw_line_graph_impl(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count,
                          bool line, bool fill) {
    int bottom = y + height - 1;

    for (int i = 0; i < (data_count < width ? data_count : width); i++) {
        int bar_y = bottom - ((data[i] * (height - 1)) / 100);
        if (bar_y < y) bar_y = y;
        if (bar_y > bottom) bar_y = bottom;

        if (fill) {
            qp_line(device, x + i, bar_y, x + i, bottom, HSV_WHITE);
        } else if (line && i > 0) {
            int last_bar_y = bottom - ((data[i - 1] * (height - 1)) / 100);
            if (last_bar_y < y) last_bar_y = y;
            if (last_bar_y > bottom) last_bar_y = bottom;
            qp_line(device, x + (i - 1), last_bar_y, x + i, bar_y, HSV_WHITE);
        } else {
            qp_line(device, x + i, bar_y, x + i, bar_y, HSV_WHITE);
        }
    }

    draw_vertical_scale(device, x, y, height, 3, 5);
    draw_horizontal_scale(device, x, bottom, width, 3, 5);
}

void draw_line_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count,
                     bool line, bool fill) {
    draw_line_graph_impl(device, x, y, width, height, data, data_count, line, fill);
}

/*
 * Area Graph
 */
void draw_area_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count) {
    draw_line_graph_impl(device, x, y, width, height, data, data_count, false, true);
}

/*
 * Dot Graph
 */
void draw_dot_graph(painter_device_t device, int x, int y, int width, int height, uint8_t data[], int data_count) {
    draw_line_graph_impl(device, x, y, width, height, data, data_count, false, false);
}

/*
 * Bar Graph
 */
void draw_bar_graph_impl(painter_device_t device, int x, int y, int width, int height, int data[], int data_count,
                         int bar_width, int bar_padding) {
    int bottom      = y + height - 1;
    int bar_start_x = x + 2;
    int bar_count = (data_count < (width / (bar_width + bar_padding)) ? data_count : width / (bar_width + bar_padding));

    for (int i = 0; i < bar_count; i++) {
        int bar_height = (data[i] * (height - 1)) / 100;
        if (bar_height > height - 1) bar_height = height - 1;
        int offset_x = bar_start_x + (i * (bar_width + bar_padding));
        qp_rect(device, offset_x, bottom - bar_height, offset_x + bar_width, bottom, HSV_WHITE, true);
    }

    draw_vertical_scale(device, x, y, height, 3, 5);
    qp_line(device, x, bottom, x + width, bottom, HSV_WHITE);
}

void draw_bar_graph(painter_device_t device, int x, int y, int width, int height, int data[], int data_count) {
    draw_bar_graph_impl(device, x, y, width, height, data, data_count, 3, 2);
}

void draw_autoscale_bar_graph(painter_device_t device, int x, int y, int width, int height, int data[],
                              int data_count) {
    int bar_padding = 3;

    if (data_count > 4) bar_padding = 2;
    if (data_count > 8) bar_padding = 1;

    int bar_width = ((width - (data_count * bar_padding)) / data_count);
    if (bar_width < 1) bar_width = 1;

    draw_bar_graph_impl(device, x, y, width, height, data, data_count, bar_width, bar_padding);
}

/*
 * Linear Gauge
 */
void draw_linear_gauge(painter_device_t device, int x, int y, int width, int height, int value) {
    int needle_x      = (value * width) / 100 + x;
    int needle_height = (3 * height) / 5;
    int needle_y      = y + (height - needle_height);

    draw_horizontal_scale(device, x, needle_y, width, 2, 10);

    // Draw frame
    qp_line(device, x, needle_y + 2, x + width - 1, needle_y + 2, HSV_WHITE);
    qp_line(device, x, y, x, needle_y + 4, HSV_WHITE);
    qp_line(device, x + width - 1, y, x + width - 1, needle_y + 4, HSV_WHITE);

    // Draw needle (triangle)
    qp_triangle(device, needle_x, needle_y, needle_x - 4, y + height, needle_x + 4, y + height, HSV_WHITE, true);
    qp_line(device, needle_x, needle_y, needle_x, y + height, HSV_BLACK);
}

/*
 * Needle Meter
 */
void draw_needle_meter(painter_device_t device, int x, int y, int width, int value) {
    int radius        = width;
    int circle_x      = x + width / 2;
    int circle_y      = y + radius;
    int needle_length = radius - 4;
    int needle_taper  = radius - 15;

    for (int i = 0; i <= 20; i++) {
        float mapped = ((i * 5 > 100 ? 100 : i * 5) - 50) * 0.01 - PI / 2;
        float xoff   = cos(mapped);
        float yoff   = sin(mapped);
        int   length = 4;
        if (i % 5 == 0) length = 10;
        qp_line(device, circle_x + (int)(xoff * radius), circle_y + (int)(yoff * radius),
                circle_x + (int)(xoff * (radius - length)), circle_y + (int)(yoff * (radius - length)), HSV_WHITE);
    }

    float mapped = ((value > 100 ? 100 : value) - 50) * 0.01 - PI / 2;
    float xoff   = cos(mapped);
    float yoff   = sin(mapped);
    qp_line(device, circle_x, circle_y, circle_x + (int)(xoff * needle_length), circle_y + (int)(yoff * needle_length),
            HSV_WHITE);
    qp_line(device, circle_x + 1, circle_y, circle_x + 1 + (int)(xoff * needle_taper),
            circle_y + (int)(yoff * needle_taper), HSV_WHITE);
    qp_line(device, circle_x - 1, circle_y, circle_x - 1 + (int)(xoff * needle_taper),
            circle_y + (int)(yoff * needle_taper), HSV_WHITE);
}

/*
 * Signal Strength
 */
void draw_signal_strength_impl(painter_device_t device, int x, int y, int width, int height, int value, int bar_width) {
    int highest = (value * (width / (1 + bar_width))) / 100;

    for (int i = 0; i < highest; i++) {
        int bar_height = (i * (1 + bar_width) * height) / width;
        qp_rect(device, x + i * (1 + bar_width), y + height - bar_height, x + i * (1 + bar_width) + bar_width,
                y + height, HSV_WHITE, true);
    }
}

void draw_signal_strength(painter_device_t device, int x, int y, int width, int height, int value) {
    draw_signal_strength_impl(device, x, y, width, height, value, 1);
}

/*
 * Thermometer
 */
void draw_thermometer(painter_device_t device, int x, int y, int width, int height, int value) {
    int thickness = 2;
    if (width > 20 && height > 20) thickness = 3;
    if (width < 8) thickness = 1;
    int corner_radius = thickness * 3;

    int bulb_radius = ((width / 2) - thickness * 2);
    if (bulb_radius < 1) bulb_radius = 1;
    int bulb_x = x + bulb_radius + thickness * 2;
    int bulb_y = y + height - bulb_radius - thickness * 2 - 1;

    int bar_width = ((width / 2) - thickness * 4);
    if (bar_width < 1) bar_width = 1;
    int bar_x          = bulb_x - bar_width / 2;
    int bar_y          = y + thickness * 2;
    int bar_bottom_y   = bulb_y - bulb_radius - thickness * 2;
    int bar_max_height = (bar_y > bar_bottom_y) ? bar_y - bar_bottom_y : bar_bottom_y - bar_y;

    int bar_height = (value * bar_max_height) / 100 + corner_radius;
    int scale_x    = bulb_x + bar_width / 2 + thickness * 3;
    int scale_y    = y + thickness * 3 - 1;

    // Outline
    draw_vertical_scale(device, scale_x, scale_y, bar_max_height, 3, 5);
    qp_rounded_rect(device, bar_x - thickness * 2, y, bar_x + bar_width + thickness * 4, y + height - bulb_radius,
                    corner_radius, HSV_WHITE, true);
    qp_circle(device, bulb_x, bulb_y, bulb_radius + thickness * 2, HSV_WHITE, true);
    qp_rounded_rect(device, bar_x - thickness * 1, y + thickness, bar_x + bar_width + thickness * 2,
                    y + height - bulb_radius - thickness * 2, corner_radius, HSV_BLACK, true);
    qp_circle(device, bulb_x, bulb_y, bulb_radius + thickness * 1, HSV_BLACK, true);

    // Inner
    qp_rect(device, bar_x, bar_bottom_y + 1, bar_x + bar_width, bar_bottom_y + 1 + bulb_radius, HSV_WHITE, true);
    qp_circle(device, bulb_x, bulb_y, bulb_radius, HSV_WHITE, true);
    qp_rounded_rect(device, bar_x, bar_bottom_y - bar_height + corner_radius, bar_x + bar_width, bar_bottom_y,
                    corner_radius, HSV_WHITE, true);
}

/*
 * Segmented Gauge
 */
void draw_segmented_gauge(painter_device_t device, int x, int y, int width, int height, int value, int segments) {
    if (segments < 1) segments = 1;
    if (segments > width / 5) segments = width / 5;

    int margin         = 2;
    int segment_width  = (width / segments) - margin;
    int highlight_to_x = (value * width) / 100;

    for (int i = 0; i < segments; i++) {
        int offset = (segment_width + margin) * i;
        if (offset < highlight_to_x) {
            qp_rect(device, x + offset, y, x + offset + segment_width, y + height, HSV_WHITE, true);
        } else {
            qp_rect(device, x + offset, y, x + offset + segment_width, y + height, HSV_WHITE, false);
        }
    }
}

/*
 * Dot Gauge
 */
void draw_dot_gauge(painter_device_t device, int x, int y, int width, int height, int value) {
    int margin         = 2;
    int segment_radius = (height / 2);
    int segments       = width / (segment_radius * 2 + margin);
    int highlight_to_x = (value * width) / 100;

    for (int i = 0; i < segments; i++) {
        int offset = ((segment_radius * 2 + margin) * i) + segment_radius;
        if (offset < highlight_to_x) {
            qp_circle(device, x + offset, y + segment_radius, segment_radius, HSV_WHITE, true);
        } else {
            qp_circle(device, x + offset, y + segment_radius, segment_radius, HSV_WHITE, false);
        }
    }
}

/*
 * Radial Gauge
 */
void draw_radial_gauge_impl(painter_device_t device, int x, int y, int radius, int value, int padding,
                            bool outer_border, bool inner_border, bool draw_line, float start_offset) {
    int   segments     = 32;
    int   inner_radius = (radius * 6) / 10;
    int   meter_radius = radius - padding;
    float segment_arc  = (2 * PI) / segments;
    float half_arc     = segment_arc / 2;
    int   fill_up_to   = (value > 100 ? 100 : value) * segments / 100;

    for (int i = 0; i < fill_up_to; i++) {
        float segment_theta = i * segment_arc + start_offset;
        qp_triangle(device, x, y, x + (int)(cos(segment_theta - half_arc) * meter_radius),
                    y + (int)(sin(segment_theta - half_arc) * meter_radius),
                    x + (int)(cos(segment_theta + half_arc) * meter_radius),
                    y + (int)(sin(segment_theta + half_arc) * meter_radius), HSV_WHITE, true);
    }

    qp_circle(device, x, y, inner_radius, HSV_BLACK, true);

    if (outer_border) qp_circle(device, x, y, radius, HSV_WHITE, false);
    if (inner_border) qp_circle(device, x, y, inner_radius - padding, HSV_WHITE, false);
    if (draw_line) {
        float x1 = cos(fill_up_to * segment_arc + start_offset);
        float y1 = sin(fill_up_to * segment_arc + start_offset);
        qp_line(device, x + (int)(x1 * radius), y + (int)(y1 * radius), x + (int)(x1 * radius * 0.4),
                y + (int)(y1 * radius * 0.4), HSV_WHITE);
    }
}

void draw_radial_gauge(painter_device_t device, int x, int y, int radius, int value, int padding) {
    draw_radial_gauge_impl(device, x, y, radius, value, padding, true, true, true, -PI / 2);
}

/*
 * Radial Dot Gauge
 */
void draw_radial_dot_gauge_impl(painter_device_t device, int x, int y, int radius, int dot_radius, int value,
                                int segments, int empty_dot_radius, float start_offset) {
    float segment_arc = (2 * PI) / segments;
    int   fill_up_to  = (value > 100 ? 100 : value) * segments / 100;

    for (int i = 0; i < segments; i++) {
        float segment_theta = i * segment_arc + start_offset;
        if (i <= fill_up_to) {
            qp_circle(device, x + (int)(cos(segment_theta) * radius), y + (int)(sin(segment_theta) * radius),
                      dot_radius, HSV_WHITE, true);
        } else {
            qp_circle(device, x + (int)(cos(segment_theta) * radius), y + (int)(sin(segment_theta) * radius),
                      empty_dot_radius, HSV_WHITE, false);
        }
    }
}

void draw_radial_dot_gauge(painter_device_t device, int x, int y, int radius, int dot_radius, int value) {
    draw_radial_dot_gauge_impl(device, x, y, radius, dot_radius, value, 8, 1, -PI / 2);
}

/*
 * Radial Segment Gauge
 */
void draw_radial_segment_gauge_impl(painter_device_t device, int x, int y, int radius, int segments, int value,
                                    int padding, bool outer_border, bool inner_border, float start_offset) {
    float segment_arc = (2 * PI) / segments;
    float half_arc    = segment_arc / (sqrt(radius) - 1);
    int   fill_up_to  = (value > 100 ? 100 : value) * segments / 100;

    for (int i = 0; i < fill_up_to; i++) {
        float segment_theta = i * segment_arc + start_offset;
        qp_triangle(device, x + (int)(cos(segment_theta) * (radius / 3.0 - padding)),
                    y + (int)(sin(segment_theta) * (radius / 3.0 - padding)),
                    x + (int)(cos(segment_theta - half_arc) * (radius - padding)),
                    y + (int)(sin(segment_theta - half_arc) * (radius - padding)),
                    x + (int)(cos(segment_theta + half_arc) * (radius - padding)),
                    y + (int)(sin(segment_theta + half_arc) * (radius - padding)), HSV_WHITE, true);
    }

    qp_circle(device, x, y, (radius / 2), HSV_BLACK, true);
    if (inner_border) qp_circle(device, x, y, ((radius / 2) - padding), HSV_WHITE, false);
    if (outer_border) qp_circle(device, x, y, radius, HSV_WHITE, false);
}

void draw_radial_segment_gauge(painter_device_t device, int x, int y, int radius, int segments, int value) {
    draw_radial_segment_gauge_impl(device, x, y, radius, segments, value, 2, true, true, -PI / 2);
}

/*
 * Radial Line Gauge
 */
void draw_radial_line_gauge_impl(painter_device_t device, int x, int y, int radius, int lines, int value,
                                 bool outer_border, bool inner_border, float start_offset) {
    float segment_arc = (2 * PI) / lines;
    // float half_arc    = segment_arc / 4;
    int fill_up_to = (value > 100 ? 100 : value) * lines / 100;

    for (int i = 0; i < fill_up_to; i++) {
        float segment_theta = i * segment_arc + start_offset;
        int   x1            = (int)(cos(segment_theta) * radius);
        int   y1            = (int)(sin(segment_theta) * radius);
        if (i <= fill_up_to) {
            qp_line(device, x, y, x + x1, y + y1, HSV_WHITE);
        }
    }

    qp_circle(device, x, y, (radius * 4) / 10, HSV_BLACK, true);
    if (outer_border) qp_circle(device, x, y, radius, HSV_WHITE, false);
    if (inner_border) qp_circle(device, x, y, (radius * 4) / 10, HSV_WHITE, false);
}

void draw_radial_line_gauge(painter_device_t device, int x, int y, int radius, int lines, int value) {
    draw_radial_line_gauge_impl(device, x, y, radius, lines, value, true, true, -PI / 2);
}

#endif
