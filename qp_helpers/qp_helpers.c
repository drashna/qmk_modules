// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp_helpers.h"

static inline uint8_t scale_value(uint8_t value, uint8_t from, uint8_t to) {
    return (value * from / to);
}

bool qp_draw_graph(painter_device_t device, uint16_t graph_x, uint16_t graph_y, uint16_t graph_width,
                   uint16_t graph_height, hsv_t primary, hsv_t background, const graph_line_t *graph_data,
                   uint8_t n_graphs, uint8_t graph_segments, uint8_t scale_to) {
    uint8_t graph_starting_index = 0;
    // if there are more segments than the graph width is wide in pixels, then set up things to only render the last
    // graph_width segments of the array.
    if (graph_segments >= graph_width) {
        graph_starting_index = graph_segments - graph_width;
        graph_segments       = graph_width;
    }
    // clear the graph area for redrawing
    if (!qp_rect(device, graph_x, graph_y, graph_x + graph_width, graph_y + graph_height, background.h, background.s,
                 background.v, true)) {
        return false;
    }

    // Draw graph axes
    if (!qp_line(device, graph_x, graph_y, graph_x, graph_y + graph_height, primary.h, primary.s, primary.v)) {
        return false;
    }
    if (!qp_line(device, graph_x, graph_y + graph_height, graph_x + graph_width, graph_y + graph_height, primary.h,
                 primary.s, primary.v)) {
        return false;
    }

    uint8_t spacing   = graph_width / (graph_segments);
    uint8_t remainder = graph_width - (graph_segments * spacing);

    // Plot graph data
    uint8_t offset = 0;
    for (uint8_t n = 0; n < n_graphs; n++) {
        for (uint8_t i = graph_starting_index; i < graph_segments; i++) {
            offset += (remainder != 0 && (i - graph_starting_index) % (graph_segments / remainder) == 0) ? 1 : 0;
            uint16_t x1 = graph_x + ((i - graph_starting_index) * spacing) + offset;
            uint16_t y1 =
                graph_y + graph_height - scale_value(graph_data[n].line_data[i], graph_height - 1, scale_to) - 1;
            if (y1 < graph_y) {
                y1 = graph_y;
            }
            uint16_t x2 = graph_x + (((i - graph_starting_index) + 1) * spacing) + offset;
            if (x2 >= graph_x + graph_width) {
                x2 = graph_x + graph_width - 1;
            }
            uint16_t y2 =
                graph_y + graph_height - scale_value(graph_data[n].line_data[i + 1], graph_height - 1, scale_to) - 1;
            if (y2 < graph_y) {
                y2 = graph_y;
            }
            switch (graph_data[n].mode) {
                case LINE:
                    if (!qp_line(device, x1, y1, x2, y2, graph_data[n].line_color.h, graph_data[n].line_color.s,
                                 graph_data[n].line_color.v)) {
                        return false;
                    }
                    break;
                case POINT:
                    if (!qp_rect(device, x1, y1, x1 + 1, y1 + 1, graph_data[n].line_color.h, graph_data[n].line_color.s,
                                 graph_data[n].line_color.v, true)) {
                        return false;
                    }
                    break;
                case SQUARE_BOX:
                    if (!qp_rect(device, x1 - 1, y1 - ((y1 < graph_y) ? 1 : 0), x1 + 1,
                                 y1 + ((y1 >= graph_y + graph_height - 1) ? ((y1 > graph_y + graph_height - 1) ? -1 : 0)
                                                                          : 1),
                                 graph_data[n].line_color.h, graph_data[n].line_color.s, graph_data[n].line_color.v,
                                 true)) {
                        return false;
                    }
                    break;
                case SQUARED_LINE:
                    if (!qp_line(device, x1, y1, x2, y1, graph_data[n].line_color.h, graph_data[n].line_color.s,
                                 graph_data[n].line_color.v)) {
                        return false;
                    }
                    if (!qp_line(device, x2, y1, x2, y2, graph_data[n].line_color.h, graph_data[n].line_color.s,
                                 graph_data[n].line_color.v)) {
                        return false;
                    }
                    break;
            }
        }
    }

    // Add markers on the y-axis for every 10 units, scaled to scale_to
    for (uint16_t i = 0; i <= scale_to; i += 10) {
        uint16_t marker_y = graph_y + graph_height - scale_value(i, graph_height - 1, scale_to);
        if (!qp_line(device, graph_x, marker_y, graph_x + 2, marker_y, primary.h, primary.s, primary.v)) {
            return false;
        }
    }

    return true;
}

#endif
