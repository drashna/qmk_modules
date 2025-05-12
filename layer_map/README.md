# Layer Map

This adds support for creating an array that is accurate to the keyboard, accounting for layers and transparency. Eg, if you want a live view of how the keyboard is actually laid out. This triggers on layer (and default layer) changes, as well as swap hand status changes and via based remapping.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/layer_map"]
}
```

This doesn't do anything by itself (other than populate the array). You'll need to check the `layer_map[rows][cols]` array for the specific keycodes. And you'll need to include `modules/drashna/layer_map/layer_map.h`.

## Helper functions

| Function Name                     | Description                                                                         |
| --------------------------------- | ----------------------------------------------------------------------------------- |
| `set_layer_map_dirty(void)`       | Marks the layer map as dirty, and needs to be updated/refreshed to reflect changes. |
| `get_layer_map_has_updated(void)` | Returns `true`/`false` based on if the layer map has been updated this loop.        |
| `set_layer_map_has_updated(bool)` | Sets the status of if the layer map has been updated.                               |
| `peek_matrix_layer_map(row, col)` | Checks the on/off state of a specific matrix position for the layer map.            |

## Layer Map Remapping

Because many split boards have matrices that don't match the layouts, you can define `LAYER_MAP_REMAPPING` in your config.h, and provide an array of modified keypos locations. This also allows for placing encoder map config into the layer map. Make sure this is in your keymap.c.

You can see this with the kyria:

```c
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
keypos_t layer_remap[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {
    { { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 } },
    { {   6,   0 }, {   5,   0 }, {   4,   0 }, {   3,   0 }, {   2,   0 }, {   1,   0 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1,   4 }, {   2,   4 }, {   3,   4 }, {   4,   4 }, {   5,   4 }, {   6,   4 } },
    { {   6,   1 }, {   5,   1 }, {   4,   1 }, {   3,   1 }, {   2,   1 }, {   1,   1 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1,   5 }, {   2,   5 }, {   3,   5 }, {   4,   5 }, {   5,   5 }, {   6,   5 } },
    { {   6,   2 }, {   5,   2 }, {   4,   2 }, {   3,   2 }, {   2,   2 }, {   1,   2 }, {   3,   3 }, {   0,   2 }, { 255, 255 }, { 255, 255 }, {   0,   6 }, {   3,   7 }, {   1,   6 }, {   2,   6 }, {   3,   6 }, {   4,   6 }, {   5,   6 }, {   6,   6 } },
    { { 255, 255 }, { 255, 255 }, { 255, 255 }, {   4,   3 }, {   2,   3 }, {   1,   3 }, {   5,   3 }, {   0,   3 }, { 255, 255 }, { 255, 255 }, {   0,   7 }, {   5,   7 }, {   1,   7 }, {   2,   7 }, {   4,   7 }, { 255, 255 }, { 255, 255 }, { 255, 255 } },
    { { 255, 255 }, {   0, 252 }, {   0, 253 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1, 252 }, {   1, 253 }, { 255, 255 } },
};
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
```

## Sample code

An example of this in action. This would also need the `code_to_name` array for the oled keylogging code, in my (drashna's) userspace ([/users/drashna/display/oled/oled_stuff.c, lines 48-68](https://github.com/drashna/qmk_userspace/blob/566c474f07969bcc10be33cedabba56550d1abff/users/drashna/display/oled/oled_stuff.c#L48-L68)).

```c
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "nlayer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE

bool oled_task_user(void) {
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
        oled_set_cursor(1, i);
        for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
            uint16_t keycode = extract_basic_keycode(layer_map[i][j], NULL, false);

            char code = 0;
            if (keycode > 0xFF) {
                if (keycode == UC_IRNY) {
                    code = 0xFD;
                } else if (keycode == UC_CLUE) {
                    code = 0xFE;
                } else if (keycode == DISPLAY_MENU) {
                    code = 0xC8;
                } else {
                    keycode = 0;
                }
            }
            if (keycode < ARRAY_SIZE(code_to_name)) {
                code = pgm_read_byte(&code_to_name[keycode]);
            }

            oled_write_char(code, peek_matrix_layer_map(i, j));
        }
    }
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
    return false;
}
```
