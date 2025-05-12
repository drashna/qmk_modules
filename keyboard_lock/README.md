This adds the hacky keyboard lock from the command feature as a module. This works by unmounting the USB driver and holding it in a placeholder, and loading it back in. This **_should not_** be used with Bluetooth or Wireless boards, as it's likely that it's using a similar trick to handle wireless.

Note that this will prevent any keyboard or mouse input from working. Console and other features not handled through the host driver strcture may still work.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/keyboard_lock"]
}
```

After this, your keymap can add `CM_KEYBOARD_LOCK_TOGGLE`, or `KEYLOCK` to any keymap position.
