# Wiggle Ball

THis module allows you to "wiggle" the trackball, mouse or other pointing device sensor to trigger a desired behavior (be it drag scrolling or something else). Just "shake" it from left to right to left again, or right to left to right again, and it will activate.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/wiggle_ball"]
}
```

## Configuration

| Setting                                | Default | Description                                                                                          |
| -------------------------------------- | ------- | ---------------------------------------------------------------------------------------------------- |
| `WIGGLE_BALL_TIMEOUT`                  | `250`   | Timeout for processing wiggling (longer than this time will end the check).                          |
| `WIGGLE_BALL_DIRECTION_SWITCH_TIMEOUT` | `150`   | Timeout between direction changes. If movement takes longer than this to reverse, it's not a wiggle. |

## Functions

- `is_wiggle_ball_enabled(void)` - Are we actively wiggling?
- `set_wiggle_ball_timeout(uint16_t timeout)` - Changes the timeout value. Setting to 0 sets to default.
- `get_wiggle_ball_timeout(void)` - Gets the current timeout value.
- `set_wiggle_ball_direction_switch_timeout(uint16_t timeout)` - Changes the direction switch timeout value.
- `get_wiggle_ball_direction_switch_timeout(void)` - Gets the current direction switch timeout.

### `wiggle_ball_state_change(bool enabled, report_mouse_t *report)`

The `wiggle_ball_state_change` function is called after the timeout period has occurred, returning both the enabled state and passing the current mouse report as a pointer to the function.

By default, this will either run rudamentary scrolling code or will set drag scrolling, if the [drag scroll module](../drag_scroll/) is enabled.

Additionally, this function is weak, so you can add alternative handling to this (such as enabling/disabling mouse acceleration, turning on or off rgb, activating display menu, etc).
