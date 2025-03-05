// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "display_menu.h"
#include "oled_driver.h"

#define snprintf_nowarn(...) (snprintf(__VA_ARGS__) < 0 ? abort() : (void)0)

extern menu_state_runtime_t menu_state_runtime;
extern menu_state_t         menu_state;

bool oled_render_menu(uint8_t col, uint8_t line, uint8_t num_of_lines, bool is_left) {
    uint8_t     scroll_offset   = 0;
    char        text_buffer[21] = {0};
    static bool last_state      = false;

    if (menu_state.is_in_menu != last_state) {
        last_state               = menu_state.is_in_menu;
        menu_state_runtime.dirty = true;
    }

    if (menu_state_runtime.dirty) {
        oled_set_cursor(col, line);
        for (uint8_t i = 0; i < num_of_lines; i++) {
            oled_advance_page(true);
        }
        menu_state_runtime.dirty = false;
    }

    if (!(menu_render_side & (1 << (uint8_t)!is_left))) {
        return false;
    }

    if (!menu_state.is_in_menu) {
        return false;
    }

    menu_entry_t *menu     = get_current_menu();
    menu_entry_t *selected = get_selected_menu_item();

    scroll_offset = get_menu_scroll_offset(menu, num_of_lines - 1);

    snprintf(text_buffer, sizeof(text_buffer), "%-20s", menu->text);

    oled_set_cursor(col, line);
    for (uint8_t i = 0; i < num_of_lines; i++) {
        oled_advance_page(true);
    }
    oled_set_cursor(col, line);
    oled_write(" ", true);
    oled_write(text_buffer, true);
    for (uint8_t i = scroll_offset; i < menu->parent.child_count && i <= (scroll_offset + (num_of_lines - 1) - 1);
         i++) {
        menu_entry_t *child   = &menu->parent.children[i];
        char          buf[19] = {0}, val[19] = {0};

        oled_set_cursor(col, line + 1 + i - scroll_offset);
        oled_write_P(PSTR(" "), false);
        if (child == selected) {
            oled_write_char(0xC2, false);
        } else {
            if (i == scroll_offset && scroll_offset > 0) {
                oled_write_char(0x18, false);
            } else if (i == scroll_offset + (num_of_lines - 2) &&
                       scroll_offset + (num_of_lines - 1) < menu->parent.child_count) {
                oled_write_char(0x19, false);
            } else {
                oled_write_char(0x20, false);
            }
        }
        snprintf(buf, sizeof(buf), "%s", child->short_text);

        if (child->flags & menu_flag_is_value) {
            child->child.display_handler(val, sizeof(val));
            if (child->flags & menu_flag_is_parent) {
                snprintf_nowarn(text_buffer, 19, "%s [%s]", buf, val);
            } else {
                snprintf_nowarn(text_buffer, 20, "%s: %s", buf, val);
            }
        } else {
            snprintf_nowarn(text_buffer, 19, "%s", buf);
        }
        oled_write(text_buffer, false);
        if (child->flags & menu_flag_is_parent) {
            oled_set_cursor(col + 20, line + 1 + i - scroll_offset);
            oled_write_char(0x10, false);
        }
        oled_advance_page(true);
    }
    return true;
}
