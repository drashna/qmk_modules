
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Audio

#ifdef AUDIO_ENABLE
bool menu_handler_audio_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            if (audio_is_on()) {
                audio_off();
            } else {
                audio_on();
            }
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_audio_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", audio_is_on() ? "on" : "off");
}

bool menu_handler_music_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            music_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_music_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_music_on() ? "on" : "off");
}

bool menu_handler_audio_clicky_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            clicky_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_audio_clicky_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_clicky_on() ? "enabled" : "disabled");
}

bool menu_handler_audio_clicky_freq(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            clicky_freq_down();
            return false;
        case menu_input_right:
            clicky_freq_up();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_audio_clicky_freq(char *text_buffer, size_t buffer_len) {
    extern float clicky_freq;
    snprintf(text_buffer, buffer_len - 1, "%.2f", (float)clicky_freq);
}

menu_entry_t audio_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Audio", "Audio", audio_enabled),
    DISPLAY_MENU_ENTRY_CHILD("Music Mode", "Music", music_enabled),
    DISPLAY_MENU_ENTRY_CHILD("Clicky", "Clicky", audio_clicky_enabled),
    DISPLAY_MENU_ENTRY_CHILD("Clicky Frequency", "Freq", audio_clicky_freq),
};
#endif // AUDIO_ENABLE
