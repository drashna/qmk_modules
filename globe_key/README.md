This is adds a "cut-down" version of the macOS Globe Key to your build.
As the full Globe Key functionality requires an Apple vendor/product ID pair, this isn't completely functional and things like F-key support may not work.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/globe_key"]
}
```

After this, your keymap can add `KC_GLOBE` (aliased to `KC_GLB`) to a keymap position.
