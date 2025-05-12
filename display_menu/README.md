# Display Menu

This adds @drashna's custom on-screen menu code as a community module. This works for both the `oled driver` feature and for `quantum painter`.

This is currently a work in progress.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/display_menu"]
}
```

Note that this will also automatically enabled the deferred execution feature.

However, there are a few assumptions that are made here. For Quantum Painter, `housekeeping_task_display_menu_kb(void)` or `housekeeping_task_display_menu_user(void)` should be used for rendering the display feature. This is so that some of the menu dirty state can be best handled.

Additionally, `process_record_display_menu_handling_user(uint16_t keycode, bool keep_processing)` is used to map keycodes to specific menu functionality (cardinal directions, select, escape, back).

There is a default menu that is populated already, but if you wish to configure your own. Add a define for `DISPLAY_MENU_ENTRY_H` that lists the location of a file that includes the `menu_entry_t root` array. This should have one item, and be a parent type.

Additionally, selections can be found at:

```c

#include "modules/drashna/display_menu/submenus/display_options.h"
#include "modules/drashna/display_menu/submenus/unicode_options.h"
#include "modules/drashna/display_menu/submenus/rgb_options.h"
#include "modules/drashna/display_menu/submenus/backlight_options.h"
#include "modules/drashna/display_menu/submenus/audio_options.h"
#include "modules/drashna/display_menu/submenus/haptic_options.h"
#include "modules/drashna/display_menu/submenus/pointing_device_options.h"
#include "modules/drashna/display_menu/submenus/keymap_config_options.h"
#include "modules/drashna/display_menu/submenus/rtc_options.h"
#include "modules/drashna/display_menu/submenus/user_settings_options.h"
#include "modules/drashna/display_menu/submenus/debugging_options.h"
```

## Rendering

If you're using QP, then you want to include `qp_render_menu.h` and call the following function to render:

```c
bool painter_render_menu(painter_device_t display, painter_font_handle_t font, uint16_t start_x, uint16_t start_y,
                         uint16_t end_x, uint16_t end_y, bool is_verbose, hsv_t primary, hsv_t secondary);
```

If you're using OLED driver, then you want to include `oled_render_menu.h` and call the following function to render:

```c
bool oled_render_menu(uint8_t col, uint8_t line, uint8_t num_of_lines, uint8_t menu_render_side);
```

Note that render side should be 1 (left), 2 (right) or 3 (both). 0 means "neither side".

## Keycodes

`DISPLAY_MENU` or `DS_MENU` is used to open up the menu or close it.
