# Display Keylogger

The Display Keylogger keeps a rolling log of recently pressed keys and exposes
it as text for a display or other UI to render. It is a data and key-processing
module; it does not configure a display or draw the log by itself.

## What it does

- Records printable key representations, applying the active Shift, Caps Lock,
  and supported AltGr behavior. Common non-printing keys are represented by
  short labels or symbols.
- Maintains the most recent entries in a fixed-length buffer. The default is
  25 characters.
- Handles Backspace by removing the newest entry. Ctrl+Backspace clears the
  log.
- Saves the log to persistent storage (EEPROM) and defers writes for 5 seconds
  after no activity.
- On split keyboards, synchronizes the log from the master half to the other
  half using the module's split-transaction ID.

## Enabling the module

Add `drashna/display_keylogger` to the keymap's `modules` list in `keymap.json`:

```json
{
    "modules": [
        "drashna/display_keylogger"
    ]
}
```

If the keymap already has a `modules` list, add the module name to that list
rather than replacing the existing entries.

The module tracks keys when `keylogger_process(keycode, record)` is called from
the key-processing path. A display integration can retrieve the formatted log
with `get_keylogger_str()` and render it when `is_keylogger_dirty()` returns
true. After updating the display, call `keylogger_set_dirty(false)` to mark the
log as rendered. Include `display_keylogger.h` to use these functions.

## Configuration and API

Set `DISPLAY_KEYLOGGER_LENGTH` at build time to change the number of retained
characters; it defaults to `25`. The module sizes its EEPROM allocation from
this value.

The public header also provides `get_keylogger_str_raw()` for the Unicode
codepoint buffer, `keycode_repr()` for keycode-label formatting, and
`keylog_shift_right()` for removing the newest entry. Split synchronization is
handled by the module and normally does not need to be called by keymaps.

This module is separate from the Drashna userspace OLED keylogger.
