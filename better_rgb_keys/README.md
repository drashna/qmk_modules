# Better RGB Keys

Enhances the standard QMK RGB hue/saturation/value keycodes so that **holding** a key continuously adjusts the colour instead of stepping by one unit per press. When the key is released the new value is flushed to EEPROM.

Works for both **RGB Matrix** (`RM_*`) and **rgblight / underglow** (`UG_*`) keycodes.

## Usage

Add the module to your `keymap.json`:

```json
{
    "modules": ["drashna/better_rgb_keys"]
}
```

No extra keycodes are needed. The module intercepts the existing QMK keycodes listed below and adds hold-to-repeat behaviour automatically.

| Keycode   | Description                        |
|-----------|------------------------------------|
| `UG_HUEU` | Underglow – increase hue           |
| `UG_HUED` | Underglow – decrease hue           |
| `UG_SATU` | Underglow – increase saturation    |
| `UG_SATD` | Underglow – decrease saturation    |
| `UG_VALU` | Underglow – increase value         |
| `UG_VALD` | Underglow – decrease value         |
| `RM_HUEU` | RGB Matrix – increase hue          |
| `RM_HUED` | RGB Matrix – decrease hue          |
| `RM_SATU` | RGB Matrix – increase saturation   |
| `RM_SATD` | RGB Matrix – decrease saturation   |
| `RM_VALU` | RGB Matrix – increase value        |
| `RM_VALD` | RGB Matrix – decrease value        |

## Configuration

| Define              | Default | Description                                          |
|---------------------|---------|------------------------------------------------------|
| `BETTER_RGB_TICK_MS`| `15`    | Milliseconds between each adjustment step while held |

## Requirements

- `RGBLIGHT_ENABLE = yes` and/or `RGB_MATRIX_ENABLE = yes`
- `DEFERRED_EXEC_ENABLE = yes` (enabled automatically by the module)
