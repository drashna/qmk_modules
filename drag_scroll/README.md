This adds the Pointing Device feature Drag Scrolling code as a community module.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/drag_scroll"]
}
```

## Configuration

| Setting            | Default | Description                                      |
|--------------------|---------|--------------------------------------------------|
| `SCROLL_DIVISOR_H` | `8`     | Default divisor to use for horizontal scrolling. |
| `SCROLL_DIVISOR_V` | `8`     | Default divisor to use for horizontal scrolling. |


## Functions

* `get_drag_scroll_h_divisor(void)` - Returns the current horizontal divisor value as a float.
* `get_drag_scroll_v_divisor(void)` - Returns the current vertical divisor value as a float.
* `set_drag_scroll_h_divisor(float divisor)` - Sets the horizontal divisor value.
* `set_drag_scroll_v_divisor(float divisor)` - Sets the vertical divisor value.
* `set_drag_scroll_divisor(float divisor)` - Sets both the horizontal and vertical divisors value.
* `get_drag_scroll_scrolling(void)` - Gets the current enable value for drag scrolling.
* `set_drag_scroll_scrolling(bool scrolling)` - Sets the enable state for drag scrolling.
