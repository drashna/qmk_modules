// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "display_menu.h"
#include "qp_render_menu.h"
#include "qp.h"

#define snprintf_nowarn(...) (snprintf(__VA_ARGS__) < 0 ? abort() : (void)0)

extern menu_state_runtime_t menu_state_runtime;
extern menu_state_t         menu_state;



/**
 * @brief Truncates text to fit within a certain width
 *
 * @param text original text
 * @param max_width max width in pixels
 * @param font font being used
 * @param from_start truncate from start or end
 * @param add_ellipses add ellipses to truncated text
 * @return char* truncated text
 */
static char* truncate_text(const char* text, uint16_t max_width, painter_font_handle_t font, bool from_start,
                    bool add_ellipses) {
    static char truncated_text[50];
    strncpy(truncated_text, text, sizeof(truncated_text) - 1);
    truncated_text[sizeof(truncated_text) - 1] = '\0';

    uint16_t text_width = qp_textwidth(font, truncated_text);
    if (text_width <= max_width) {
        return truncated_text;
    }

    size_t      len            = strlen(truncated_text);
    const char* ellipses       = "...";
    uint16_t    ellipses_width = add_ellipses ? qp_textwidth(font, ellipses) : 0;

    if (from_start) {
        size_t start_index = 0;
        while (start_index < len && text_width > max_width - ellipses_width) {
            start_index++;
            text_width = qp_textwidth(font, truncated_text + start_index);
        }

        if (add_ellipses) {
            char temp[75];
            snprintf(temp, sizeof(temp), "%s%s", ellipses, truncated_text + start_index);
            strncpy(truncated_text, temp, sizeof(truncated_text) - 1);
            truncated_text[sizeof(truncated_text) - 1] = '\0';
        } else {
            memmove(truncated_text, truncated_text + start_index, len - start_index + 1);
        }
    } else {
        while (len > 0 && text_width > max_width - ellipses_width) {
            len--;
            truncated_text[len] = '\0';
            text_width          = qp_textwidth(font, truncated_text);
        }

        if (add_ellipses) {
            snprintf(truncated_text + len, sizeof(truncated_text) - len, "%s", ellipses);
        }
    }

    return truncated_text;
}

bool painter_render_menu(painter_device_t display, painter_font_handle_t font, uint16_t start_x, uint16_t start_y,
                         uint16_t width, uint16_t height, bool is_thicc, dual_hsv_t hsv) {
    static menu_state_t last_state;
    uint8_t             scroll_offset = 0;

    if (memcmp(&last_state, &menu_state, sizeof(menu_state_t)) == 0 &&
        menu_state_runtime.has_rendered) {
        return menu_state.is_in_menu;
    }

    menu_state_runtime.dirty = false;
    memcpy(&last_state, &menu_state, sizeof(menu_state_t));

    uint16_t render_width = width - start_x;

    if (menu_state.is_in_menu) {
        qp_rect(display, start_x, start_y, render_width - 1, height - 1, 0, 0, 0, true);

        menu_entry_t *menu     = get_current_menu();
        menu_entry_t *selected = get_selected_menu_item();

        uint16_t y = start_y;
        qp_rect(display, start_x, y, render_width, y + 6 + font->line_height + 2, hsv.primary.h, hsv.primary.s,
                hsv.primary.v, true);
        qp_drawtext_recolor(display, start_x + 4, y + 4, font, menu->text, 0, 0, 0, hsv.primary.h, hsv.primary.s,
                            hsv.primary.v);
        y += font->line_height + 8;

        uint8_t visible_entries = (height - y) / (font->line_height + 5);

        scroll_offset = get_menu_scroll_offset(menu, visible_entries);

        for (uint8_t i = scroll_offset; i < menu->parent.child_count && i <= (scroll_offset + visible_entries - 1);
             i++) {
            y += 3;
            menu_entry_t *child = &menu->parent.children[i];
            uint16_t      x     = start_x + 2 + qp_textwidth(font, ">");
            if (child == selected) {
                qp_rect(display, start_x, y - 2, render_width, y + font->line_height + 1, hsv.secondary.h,
                        hsv.secondary.s, hsv.secondary.v, true);
                qp_drawtext_recolor(display, start_x + 1, y, font, ">", 0, 0, 0, hsv.secondary.h, hsv.secondary.s,
                                    hsv.secondary.v);
                x += qp_drawtext_recolor(
                    display, x, y, font,
                    truncate_text(is_thicc ? child->text : child->short_text, render_width, font, false, true), 0, 0, 0,
                    hsv.secondary.h, hsv.secondary.s, hsv.secondary.v);
            } else {
                if ((i == scroll_offset && scroll_offset > 0) ||
                    (i == scroll_offset + visible_entries - 1 &&
                     scroll_offset + visible_entries < menu->parent.child_count)) {
                    qp_drawtext_recolor(display, start_x + 1, y, font, "+", hsv.primary.h, hsv.primary.s, hsv.primary.v,
                                        0, 255, 0);
                }
                x += qp_drawtext_recolor(
                    display, x, y, font,
                    truncate_text(is_thicc ? child->text : child->short_text, render_width - x, font, false, true),
                    hsv.primary.h, hsv.primary.s, hsv.primary.v, 0, 255, 0);
            }
            if (child->flags & menu_flag_is_value) {
                char buf[32] = {0}, val[29] = {0};
                child->child.display_handler(val, sizeof(val));

                if (child->flags & menu_flag_is_parent) {
                    snprintf(buf, sizeof(buf), " [%s]", val);
                } else {
                    snprintf(buf, sizeof(buf), ": %s", val);
                }
                // TODO: fix text truncation with elipses for super short values
                if (child == selected) {
                    qp_drawtext_recolor(display, x, y, font, truncate_text(buf, render_width - x, font, false, false),
                                        0, 0, 0, hsv.secondary.h, hsv.secondary.s, hsv.secondary.v);
                } else {
                    qp_drawtext_recolor(display, x, y, font, truncate_text(buf, render_width - x, font, false, false),
                                        hsv.primary.h, hsv.primary.s, hsv.primary.v, 0, 0, 0);
                }
            }
            if (child->flags & menu_flag_is_parent) {
                if (child == selected) {
                    qp_drawtext_recolor(display, render_width - (qp_textwidth(font, ">") + 2), y, font, ">", 0, 0, 0,
                                        hsv.secondary.h, hsv.secondary.s, hsv.secondary.v);
                } else {
                    qp_drawtext_recolor(display, render_width - (qp_textwidth(font, ">") + 2), y, font, ">",
                                        hsv.primary.h, hsv.primary.s, hsv.primary.v, 0, 0, 0);
                }
            }
            y += font->line_height + 2;
            qp_rect(display, start_x, y, render_width, y, hsv.primary.h, hsv.primary.s, hsv.primary.v, true);
        }
        return true;
    }
    return false;
}
