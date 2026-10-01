#include "submenus/unicode_options.h"
#include "submenus/rgb_options.h"
#include "submenus/backlight_options.h"
#include "submenus/audio_options.h"
#include "submenus/haptic_options.h"
#include "submenus/keymap_config_options.h"
#include "submenus/debugging_options.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root menu

__attribute__((weak)) menu_entry_t root_entries[] = {
#ifdef UNICODE_COMMON_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("Unicode Settings", "Unicode", unicode_entries, NULL, unicode),
#endif // UNICODE_COMMON_ENABLE
#ifdef RGB_MATRIX_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("RGB Matrix Settings", "RGB Matrix", rgb_matrix_entries, NULL, rm_enabled),
#endif // RGB_MATRIX_ENABLE
#ifdef RGBLIGHT_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("RGB Light Settings", "RGB Light", rgb_light_entries, NULL, rgbenabled),
#endif // RGBLIGHT_ENABLE
#ifdef BACKLIGHT_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("Backlight Settings", "Backlight", backlight_entries, NULL, bl_level),
#endif // BACKLIGHT_ENABLE
#ifdef AUDIO_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("Audio Settings", "Audio", audio_entries, NULL, audio_enabled),
#endif // AUDIO_ENABLE
#if defined(HAPTIC_ENABLE)
    DISPLAY_MENU_ENTRY_MULTI("Haptic Settings", "Haptics", haptic_entries, NULL, haptic_enabled),
#endif // HAPTIC_ENABLE
    DISPLAY_MENU_ENTRY_PARENT("Keymap Settings", "Keymap", keymap_config_entries, NULL),
    DISPLAY_MENU_ENTRY_PARENT("Debug Settings", "Debug", debug_entries, NULL),
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root Title

__attribute__((weak)) menu_entry_t root = {
    .flags              = menu_flag_is_parent,
    .text               = "Configuration",
    .short_text         = "Config",
    .parent.children    = root_entries,
    .parent.child_count = ARRAY_SIZE(root_entries),
};
