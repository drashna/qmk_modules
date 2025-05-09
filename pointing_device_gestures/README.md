# Pointing Device Gestures

This adds simple gesture support to the pointing device feature, lovingly lifted from @tzarc's userspace.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/pointing_device_gestures"]
}
```

And adding an array for the gestures.  The array is for the cardinal and ordinal directions (eg, N, NE, E, SE, etc), starting from north and going clockwise.

```c
const uint16_t PROGMEM pointing_device_gestures[NUM_GESTURE_DIRECTIONS] =
    GESTURES_CARDNAL_AND_ORDINAL_DIRECTIONS(KC_UP, KC_1, KC_RIGHT, KC_2, KC_DOWN, KC_3, KC_LEFT, KC_4);
```

## Configuration

| Setting                              | Default | Description                                            |
|--------------------------------------|---------|--------------------------------------------------------|
| `POINTING_DEVICE_GESTURES_TIMEOUT`   | `1500`  | The default timeout for tap/timed out gestures.        |
| `POINTING_DEVICE_GESTURES_THRESHOLD` | `500`   | The minimum threshold for triggering gesture movement. |

## Keycodes

| Keycode                 | Short code | Description                                                            |
|-------------------------|------------|------------------------------------------------------------------------|
| `CM_MOUSE_GESTURE_TAP`  | `GST_TAP`  | Starts timer for gesture movement.  Timees out after time value.       |
| `CM_MOUSE_GESTURE_HOLD` | `GST_HLD`  | While held, listes for gestures, and processes gesture when releasing. |

## Functions

* `pointing_device_gestures_is_started(void)` - Returns true/false based on if the gesture is being checked.
* `pointing_device_gestures_start(void)` - Starts the gesture and listens for movement.
* `pointing_device_gestures_end(void)` - Ends the gesture and processes the gesture.
* `pointing_device_gestures_cancel(void)` - Ends the gesture but doesn't do any processing of the gesture
* `pointing_device_gestures_timed_start(void)` - Triggers timed start (`CM_MOUSE_GESTURE_TAP`) for gestures.

* `pointing_device_gestures_trigger(uint8_t direction)` - Triggeres the gesture based on direction.  Is defined weakly, so it can be replaced with custom functionality (such as enabling drag scroll or for quantum functions).
* `pointing_device_gestures_get_keycode(uint8_t direction)` - Gets the keycode to use for the direction triggered (uses `tap_code16`)

* `pointing_device_gestures_set_timeout(uint16_t timeout)` - Sets the timeout for gesture tap.
* `pointing_device_gestures_get_timeout(void)` - Gets the current timeout for gesture tap.
* `pointing_device_gestures_set_threshold(uint16_t threshold)` - Sets the minimum threshold for gesture to be processed.
* `pointing_device_gestures_get_threshold(void)` - Gets the current threshold value.
