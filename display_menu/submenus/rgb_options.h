////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB Matrix

#ifdef RGB_MATRIX_ENABLE
bool menu_handler_rm_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_matrix_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgb_matrix_is_enabled() ? "on" : "off");
}

bool menu_handler_rm_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_step_reverse();
            return false;
        case menu_input_right:
            rgb_matrix_step();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgb_matrix_get_effect_name());
}

bool menu_handler_rm_hue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_hue();
            return false;
        case menu_input_right:
            rgb_matrix_increase_hue();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_hue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_hue());
}

bool menu_handler_rm_sat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_sat();
            return false;
        case menu_input_right:
            rgb_matrix_increase_sat();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_sat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_sat());
}

bool menu_handler_rm_val(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_val();
#    if defined(RGBLIGHT_CUSTOM)
            rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
#    endif
            return false;
        case menu_input_right:
            rgb_matrix_increase_val();
#    if defined(RGBLIGHT_CUSTOM)
            rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
#    endif
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_val(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_val());
}

bool menu_handler_rm_speed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_speed();
            return false;
        case menu_input_right:
            rgb_matrix_increase_speed();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rm_speed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_speed());
}

menu_entry_t rgb_matrix_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("RGB Enabled", "Enabled", rm_enabled),
    DISPLAY_MENU_ENTRY_CHILD("RGB Mode", "Mode", rm_mode),
    DISPLAY_MENU_ENTRY_CHILD("RGB Hue", "Hue", rm_hue),
    DISPLAY_MENU_ENTRY_CHILD("RGB Saturation", "Sat", rm_sat),
    DISPLAY_MENU_ENTRY_CHILD("RGB Value", "Val", rm_val),
    DISPLAY_MENU_ENTRY_CHILD("RGB Speed", "Speed", rm_speed),
};
#endif // RGB_MATRIX_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB Light

#ifdef RGBLIGHT_ENABLE
bool menu_handler_rgbenabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgblight_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbenabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgblight_is_enabled() ? "on" : "off");
}

bool menu_handler_rgbmode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_step_reverse();
            return false;
        case menu_input_right:
            rgblight_step();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbmode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgblight_get_effect_name());
}

bool menu_handler_rgbhue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_hue();
            return false;
        case menu_input_right:
            rgblight_increase_hue();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbhue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_hue());
}

bool menu_handler_rgbsat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_sat();
            return false;
        case menu_input_right:
            rgblight_increase_sat();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbsat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_sat());
}

bool menu_handler_rgbval(menu_input_t input) {
#    if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    return menu_handler_rm_val(input);
#    endif
    switch (input) {
        case menu_input_left:
            rgblight_decrease_val();
            return false;
        case menu_input_right:
            rgblight_increase_val();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbval(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_val());
}

bool menu_handler_rgbspeed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_speed();
            return false;
        case menu_input_right:
            rgblight_increase_speed();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_rgbspeed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_speed());
}

menu_entry_t rgb_light_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("RGB Enabled", "Enabled", rgbenabled),
    DISPLAY_MENU_ENTRY_CHILD("RGB Mode", "Mode", rgbmode),
    DISPLAY_MENU_ENTRY_CHILD("RGB Hue", "Hue", rgbhue),
    DISPLAY_MENU_ENTRY_CHILD("RGB Saturation", "Sat", rgbsat),
    DISPLAY_MENU_ENTRY_CHILD("RGB Value", "Val", rgbval),
    DISPLAY_MENU_ENTRY_CHILD("RGB Speed", "Speed", rgbspeed),
};
#endif // RGBLIGHT_ENABLE
