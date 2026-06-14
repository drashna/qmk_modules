# FT6x36 Touch Sensor Module

Driver and touch-data adapter for FocalTech FT6206 / FT6236 / FT6336 family controllers over I2C.

This module supports two usage patterns:

1. Absolute touch data (always available): receive raw/rotated touch coordinates via callbacks or polling.
2. Pointing-device emulation (optional): convert touch movement and taps into mouse reports when `POINTING_DEVICE_ENABLE` is enabled.

## Requirements

- I2C bus available on the MCU
- Module added to your userspace `qmk.json` modules list

`POINTING_DEVICE_ENABLE = yes` is optional and only required if you want mouse emulation.

## Runtime Flow

- The module initializes the controller in `keyboard_post_init_ft6x36_touch()`.
- Touch data is read and cached each cycle in `housekeeping_task_ft6x36_touch()`.
- `ft6x36_touch_event_kb()` / `ft6x36_touch_event_user()` are called with the latest touch snapshot.
- If pointing device is enabled, `pointing_device_task_ft6x36_touch()` consumes the same cached snapshot.

## Configuration

### Minimal `config.h`

```c
// I2C address (default 0x38)
// #define FT6X36_I2C_ADDRESS 0x38

// I2C timeout in ms (default 20)
// #define FT6X36_I2C_TIMEOUT 20

// Optional reset pin (active-low)
// #define FT6X36_RESET_PIN B5
// #define FT6X36_RESET_DURATION_MS 5
// #define FT6X36_RESET_SETTLE_MS   300

// Optional INT pin (active-low)
// #define FT6X36_INT_PIN B4

// Rotation bounds used by 180-degree transform:
// #define FT6X36_TOUCH_ROTATE_180
// #define FT6X36_TOUCH_MAX_X 320
// #define FT6X36_TOUCH_MAX_Y 480
```

### Tunables

| Define                           | Default  | Description                                                  |
|----------------------------------|----------|--------------------------------------------------------------|
| `FT6X36_I2C_ADDRESS`             | `0x38`   | 7-bit I2C address                                            |
| `FT6X36_I2C_TIMEOUT`             | `20`     | I2C timeout in ms                                            |
| `FT6X36_RESET_PIN`               | *(none)* | MCU pin wired to RSTN (active-low)                           |
| `FT6X36_RESET_DURATION_MS`       | `5`      | Time to hold reset low (ms)                                  |
| `FT6X36_RESET_SETTLE_MS`         | `300`    | Boot settle delay after reset (ms)                           |
| `FT6X36_INT_PIN`                 | *(none)* | MCU pin wired to INT (active-low); skips I2C reads when idle |
| `FT6X36_TOUCH_SCALE_NUMERATOR`   | `2`      | Mouse delta scale numerator                                  |
| `FT6X36_TOUCH_SCALE_DENOMINATOR` | `1`      | Mouse delta scale denominator                                |
| `FT6X36_TAP_TERM_MS`             | `200`    | Max tap duration for click detection                         |
| `FT6X36_TAP_MAX_DISPLACEMENT`    | `30`     | Max finger movement for a tap                                |
| `FT6X36_TOUCH_ROTATE_180`        | *(none)* | Enable 180-degree coordinate transform when non-zero         |
| `FT6X36_TOUCH_MAX_X`             | `320`    | X upper bound used in 180-degree transform (exclusive)       |
| `FT6X36_TOUCH_MAX_Y`             | `480`    | Y upper bound used in 180-degree transform (exclusive)       |

## Absolute Touch API

Use either callbacks or polling:

```c
bool                ft6x36_init(void);
bool                ft6x36_is_connected(void);
void                ft6x36_reset(void);

bool ft6x36_touch_get_enabled(void);
void ft6x36_touch_set_enabled(bool enable);

bool ft6x36_pointing_get_enabled(void);
void ft6x36_pointing_set_enabled(bool enable);

ft6x36_touch_data_t ft6x36_read_touch(void);
ft6x36_touch_data_t ft6x36_get_last_touch(void);

void ft6x36_touch_event_kb(ft6x36_touch_data_t data);
void ft6x36_touch_event_user(ft6x36_touch_data_t data);
```

Implement `ft6x36_touch_event_user(...)` in your keymap/user code to consume absolute coordinates each housekeeping cycle.

### Data Types

`ft6x36_touch_data_t` contains:

- `touch_count` (0-2)
- `gesture`
- `p[0]` and `p[1]` touch points (`x`, `y`, `event`, `id`, `valid`)

## Pointing-Device Behavior (Optional)

When `POINTING_DEVICE_ENABLE` is set:

- Single-finger drag: relative cursor movement
- Single-finger tap: left click
- Two-finger tap: right click
- Runtime toggle: call `ft6x36_pointing_set_enabled(false)` to keep reading absolute touch data while suppressing mouse report output

When `POINTING_DEVICE_ENABLE` is not set, all absolute touch APIs remain available and functional.
