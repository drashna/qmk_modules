# FT6x36 Touch Sensor Module

Driver and mouse-behaviour adapter for the FocalTech FT6206 / FT6236 / FT6336 family of capacitive touch controllers, communicating over I2C.

## Requirements

* `POINTING_DEVICE_ENABLE = yes` in your keyboard's `rules.mk` (or keymap)
* An I2C bus available on the MCU

## Usage

Add the module to `qmk.json` / `modules` section of your userspace, then enable pointing device in your keyboard configuration.

### Minimal `config.h`

```c
// I2C address (default 0x38 – only change if hardware address pins differ)
// #define FT6X36_I2C_ADDRESS 0x38

// I2C timeout in ms (default 20)
// #define FT6X36_I2C_TIMEOUT 20

// Optional: MCU pin wired to the RSTN pad (active-low hardware reset)
// Without this the reset step is skipped entirely.
// #define FT6X36_RESET_PIN B5
// #define FT6X36_RESET_DURATION_MS 5   // hold-low time (default 5 ms)
// #define FT6X36_RESET_SETTLE_MS   300 // boot settle time (default 300 ms)
```

### Tuning

| Define                           | Default  | Description                                             |
|----------------------------------|----------|---------------------------------------------------------|
| `FT6X36_I2C_ADDRESS`             | `0x38`   | 7-bit I2C address                                       |
| `FT6X36_I2C_TIMEOUT`             | `20`     | I2C timeout (ms)                                        |
| `FT6X36_RESET_PIN`               | *(none)* | MCU pin connected to the sensor RSTN line (active-low)  |
| `FT6X36_RESET_DURATION_MS`       | `5`      | How long to hold RSTN low (ms)                          |
| `FT6X36_RESET_SETTLE_MS`         | `300`    | Delay after releasing RSTN before first I2C access (ms) || `FT6X36_INT_PIN`                 | *(none)*| MCU pin connected to the sensor INT line (active-low); enables trigger mode and skips I2C when no touch pending || `FT6X36_TOUCH_SCALE_NUMERATOR`   | `1`      | Scale numerator for mouse delta                         |
| `FT6X36_TOUCH_SCALE_DENOMINATOR` | `4`      | Scale denominator – larger = slower cursor              |
| `FT6X36_TAP_TERM_MS`             | `200`    | Max tap duration (ms) for click detection               |
| `FT6X36_TAP_MAX_DISPLACEMENT`    | `30`     | Max finger travel (sensor units) to count as tap        |

## Behaviour

| Gesture            | Action                                  |
|--------------------|-----------------------------------------|
| Single finger drag | Moves the mouse cursor (relative delta) |
| Single finger tap  | Left mouse button click                 |
| Two-finger tap     | Right mouse button click                |

## API

```c
// Check whether the sensor was detected at boot
bool ft6x36_is_connected(void);

// Runtime enable/disable (persists until reset)
bool ft6x36_touch_get_enabled(void);
void ft6x36_touch_set_enabled(bool enable);

// Read raw touch data (normally called internally)
ft6x36_touch_data_t ft6x36_read_touch(void);

// Pulse RSTN low then wait for the sensor to boot.
// Called automatically by ft6x36_init(); only needed if re-initialising at runtime.
// No-op if FT6X36_RESET_PIN is not defined.
void ft6x36_reset(void);
```
