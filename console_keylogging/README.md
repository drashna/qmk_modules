# Console Keylogging

Adds the console keylogging from the FAQ Debugging example as a community module.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/console_keylogging"]
}
```

## Information

This will print out the following type logs:

```
KL: 0x0000, col:  0, row:  0, pressed: 1, time:    42, int: 0, count: 0
```

Pressed is a boolean, so 0 or 1, and int is "interupted", and is boolean, as well. Count only increments on tap-hold type keys.

## Functions

-   `console_keylogger_set_enabled(bool enable)` - Enables or disables the keylogging, in case you don't want to constantly print to console.
-   `console_keylogger_get_enabled()` - Returns the enabled/disabled state of the keylogger
