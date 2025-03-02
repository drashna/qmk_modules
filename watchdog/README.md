This enables hardware Watchdog functionality on ARM based controllers.  This does require hardware support and associated config for your controller.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/watchdog"]
}
```
