#pragma once

#include <qp.h>
#include <stdlib.h>

/**
 * @brief Renders a simple graph based on uint8_t[] array.
 *
 * @param device quantum painter device to write to
 * @param graph_x x offset to start rendering at
 * @param graph_y y offset to start rendering at
 * @param graph_width width of the graph in pixels
 * @param graph_height height of the graph in pixels
 * @param primary primary color for the graph (used for axis color)
 * @param secondary secondary color for the graph (used for the graph line color)
 * @param background background color for the graph (used for clearing the graph area)
 * @param graph_data array of uint8_t data to plot
 * @param graph_segments number of segments in the graph_data array
 * @param scale_to maximum value to scale the graph to
 * @return true if the graph was drawn successfully
 * @return false if the graph could not be drawn
 */
bool qp_draw_graph(painter_device_t device, uint16_t graph_x, uint16_t graph_y, uint16_t graph_width,
                   uint16_t graph_height, hsv_t primary, hsv_t secondary, hsv_t background, uint8_t* graph_data,
                   uint8_t graph_segments, uint8_t scale_to);
