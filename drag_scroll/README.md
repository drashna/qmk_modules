This module adds drag scrolling support for pointing devices in QMK.

When enabled, pointer movement is converted into wheel scrolling:

- `x` movement becomes horizontal scroll (`h`)
- `y` movement becomes vertical scroll (`v`)
- Pointer movement is suppressed while drag scrolling is active

Enable it in your keymap by adding the following to `keymap.json`:

```json
{
    "modules": ["drashna/drag_scroll"]
}
```

## Configuration

| Setting            | Default | Description                                      |
| ------------------ | ------- | ------------------------------------------------ |
| `SCROLL_DIVISOR_H` | `8`     | Default divisor to use for horizontal scrolling. |
| `SCROLL_DIVISOR_V` | `8`     | Default divisor to use for vertical scrolling.   |

Larger divisor values reduce scroll speed. Smaller values increase scroll speed.

## Keycodes

- `DRAG_SCROLL_TOGGLE`: Toggles drag scrolling on key press.
- `DRAG_SCROLL_MOMENTARY`: Enables drag scrolling while the key is held.

## Functions

- `get_drag_scroll_h_divisor(void)` - Returns the current horizontal divisor value as a int8_t.
- `get_drag_scroll_v_divisor(void)` - Returns the current vertical divisor value as a int8_t.
- `set_drag_scroll_h_divisor(int8_t divisor)` - Sets the horizontal divisor value.
- `set_drag_scroll_v_divisor(int8_t divisor)` - Sets the vertical divisor value.
- `set_drag_scroll_divisor(int8_t divisor)` - Sets both the horizontal and vertical divisors value.
- `get_drag_scroll_scrolling(void)` - Gets the current enable value for drag scrolling.
- `set_drag_scroll_scrolling(bool scrolling)` - Sets the enable state for drag scrolling.

### Optional Callbacks

These weak hooks can be overridden for integration with RGB/OLED/UI state handling:

- `bool set_drag_scroll_scrolling_kb(bool scrolling)` - ran on state change for drag scroll
- `bool set_drag_scroll_scrolling_user(bool scrolling)` - ran on state change for drag scroll
- `report_mouse_t pointing_device_task_drag_scroll_kb(report_mouse_t mouse_report)` - ran after scrolling conversion
- `report_mouse_t pointing_device_task_drag_scroll_user(report_mouse_t mouse_report)` - ran after scrolling conversion
