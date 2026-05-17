# Guarded Reset

Replaces the immediate `QK_BOOT` (bootloader), `QK_RBT` (reboot), and `EE_CLR` (EEPROM clear) keycodes with versions that require the key to be held for a configurable duration before the action fires.  Accidental presses no longer cause an unintended reset.

## Usage

Add the module to your `keymap.json`:

```json
{
    "modules": [
        "drashna/hold_guarded_keycode"
    ]
}
```

The keycodes `QK_BOOT` / `QK_RBT` / `EE_CLR` work as normal in your keymap –
no changes to your keymap layout are required.

## Configuration

| Define | Default | Description |
|--------|---------|-------------|
| `GUARDED_RESET_HOLD_MS` | `3000`  | Hold duration in milliseconds before action fires. |

## How it works

`process_record_guarded_reset` intercepts `QK_BOOT`, `QK_RBT`, and `EE_CLR` before the normal `process_quantum` handler.  On press it schedules a deferred callback; on release it cancels it.
