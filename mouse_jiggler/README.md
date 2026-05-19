# Mouse Jiggler

This module periodically sends small mouse movements via deferred execution to prevent the host from going idle. It supports multiple movement patterns and optional intro/outro animation sequences that play when the jiggler is toggled on and off.

Originally cannibalized from [getreuer's mouse jiggler](https://getreuer.info/posts/keyboards/macros3/index.html#a-mouse-jiggler).

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/mouse_jiggler"]
}
```

After this, add `COMMUNITY_MODULE_MOUSE_JIGGLER_TOGGLE` (aliased to `CM_MSJG`) to any keymap position to toggle the jiggler on and off.

## Configuration

The following defines can be set in `config.h` to change the startup defaults. All values are also adjustable at runtime via the functions listed below.

| Setting                     | Default                             | Description                                                                            |
|-----------------------------|-------------------------------------|----------------------------------------------------------------------------------------|
| `PD_JIGGLER_PATTERN`        | `PD_JIGGLER_PATTERN_XLINE`          | Initial primary movement pattern.                                                      |
| `PD_JIGGLER_PATTERN_INTRO`  | `PD_JIGGLER_PATTERN_CIRCLESMALL`    | Initial intro animation pattern played when the jiggler starts.                        |
| `PD_JIGGLER_PATTERN_ENDING` | `PD_JIGGLER_PATTERN_CIRCLECCWSMALL` | Initial outro animation pattern played when the jiggler stops.                         |
| `PD_JIGGLER_INTRO_TIMEOUT`  | `1000`                              | Duration in milliseconds before the intro/outro animation is cancelled automatically.  |
| `PD_JIGGLER_BACKOFF`        | `5`                                 | Seconds to delay the next jiggler tick after any real key or pointing-device activity. |
| `PD_JIGGLER_NOINTRO`        | *(not set)*                         | Define this to disable intro/outro animations at startup.                              |
| `PD_JIGGLER_AUTOSTOP`       | *(not set)*                         | Define this to start with autostop enabled (also settable at runtime via `jiggler_set_autostop()`). |

## Movement Patterns

The following pattern identifiers are available as `pd_jiggler_pattern_t` enum values:

| Pattern                             | Value | Description                            |
|-------------------------------------|-------|----------------------------------------|
| `PD_JIGGLER_PATTERN_SUBTLE`         | `1`   | Small, irregular random-delay jitter.  |
| `PD_JIGGLER_PATTERN_XLINE`          | `2`   | Horizontal (X-axis only) oscillation.  |
| `PD_JIGGLER_PATTERN_YLINE`          | `3`   | Vertical (Y-axis only) oscillation.    |
| `PD_JIGGLER_PATTERN_CIRCLE`         | `4`   | Clockwise circle, normal size.         |
| `PD_JIGGLER_PATTERN_CIRCLESMALL`    | `5`   | Clockwise circle, small size.          |
| `PD_JIGGLER_PATTERN_CIRCLECCW`      | `6`   | Counter-clockwise circle, normal size. |
| `PD_JIGGLER_PATTERN_CIRCLECCWSMALL` | `7`   | Counter-clockwise circle, small size.  |
| `PD_JIGGLER_PATTERN_FIGURE`         | `8`   | Large figure-eight path.               |
| `PD_JIGGLER_PATTERN_SQUARE`         | `9`   | Square path.                           |

## Functions

### State

- `jiggler_get_state(void)` - Returns `0` when inactive, `1` when the main loop is running, `2` when the intro/outro is also active.
- `jiggler_start(void)` - Starts the jiggler and plays the intro animation (if enabled).
- `jiggler_end(void)` - Stops the jiggler and plays the outro animation (if enabled).
- `jiggler_toggle(void)` - Toggles the jiggler on or off.
- `jiggle_delay(uint32_t delay_sec)` - Pushes the next jiggler tick forward by `delay_sec` seconds.

### Pattern

- `jiggler_get_pattern(void)` - Returns the current primary movement pattern as a `pd_jiggler_pattern_t`.
- `jiggler_set_pattern(pd_jiggler_pattern_t pattern)` - Sets the primary movement pattern. Takes effect the next time the jiggler starts. Invalid patterns are ignored.
- `jiggler_get_pattern_intro(void)` - Returns the current intro animation pattern.
- `jiggler_set_pattern_intro(pd_jiggler_pattern_t pattern)` - Sets the intro animation pattern.
- `jiggler_get_pattern_ending(void)` - Returns the current outro animation pattern.
- `jiggler_set_pattern_ending(pd_jiggler_pattern_t pattern)` - Sets the outro animation pattern.

### Intro / Outro

- `jiggler_get_intro_enabled(void)` - Returns `true` if intro/outro animations are enabled.
- `jiggler_set_intro_enabled(bool enabled)` - Enables or disables intro/outro animations at runtime.
- `jiggler_get_intro_timeout(void)` - Returns the current intro/outro timeout in milliseconds (`0` = disabled).
- `jiggler_set_intro_timeout(uint32_t timeout_ms)` - Sets the intro/outro timeout. Pass `0` to disable.

### Backoff

- `jiggler_get_backoff(void)` - Returns the current backoff delay in seconds.
- `jiggler_set_backoff(uint32_t backoff_sec)` - Sets the backoff delay and immediately applies it to the current cycle. Pass `0` to disable.

### Autostop

- `jiggler_get_autostop(void)` - Returns `true` if the jiggler will automatically stop on any keypress.
- `jiggler_set_autostop(bool enabled)` - Enables or disables autostop at runtime.
