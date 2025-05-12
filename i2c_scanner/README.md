This adds the I2C scanner code as a community module.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/i2c_scanner"]
}
```

## Functions

-   `i2c_scanner_set_enabled(bool enable)` - Enables or disables the i2c scanner, in case you don't want to constantly print to console.
-   `i2c_scanner_get_enabled()` - Returns the enabled/disabled state of the i2c scanner.
