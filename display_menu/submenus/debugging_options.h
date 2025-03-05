
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debugging

bool menu_handler_debugging_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_enable = !debug_enable;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_debugging_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_enable ? "enabled" : "disabled");
}

bool menu_handler_keyboard_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_keyboard = !debug_keyboard;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keyboard_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_keyboard ? "enabled" : "disabled");
}

bool menu_handler_matrix_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_matrix = !debug_matrix;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_matrix_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_matrix ? "enabled" : "disabled");
}

bool menu_handler_mouse_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_mouse = !debug_mouse;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_mouse ? "enabled" : "disabled");
}

menu_entry_t debug_entries[] = {
    MENU_ENTRY_CHILD("Debugging", "Enabled", debugging_enable), // force formatting
    MENU_ENTRY_CHILD("Keyboard Debugging", "Keeb", keyboard_debugging),
    MENU_ENTRY_CHILD("Matrix Debugging", "Matrix", matrix_debugging),
    MENU_ENTRY_CHILD("Mouse Debugging", "Mouse", mouse_debugging),
};
