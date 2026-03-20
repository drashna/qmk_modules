This is adds a "do not disturb" key to your build.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/dnd_key"]
}
```

After this, your keymap can add `KC_DO_NOT_DISTURB` (aliased to `KC_DND`) to a keymap position.
