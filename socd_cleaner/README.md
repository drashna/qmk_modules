This is adapted from getreuer's [SOCD Cleaner implementation](https://getreuer.info/posts/keyboards/socd-cleaner/), moved to a community module for validation purposes.

It can be integrated into your keymap by adding the following to your `keymap.c`:

```c
//----------------------------------------------------------
// SOCD Cleaner (community module)
#ifdef COMMUNITY_MODULE_SOCD_CLEANER_ENABLE
socd_cleaner_t socd[] = {
    {{KC_W, KC_S}, SOCD_CLEANER_LAST},
    {{KC_A, KC_D}, SOCD_CLEANER_LAST},
};
#endif // COMMUNITY_MODULE_SOCD_CLEANER_ENABLE
```

And adding the following to the list of modules in your `keymap.json`:

```json
{
    "modules": ["drashna/socd_cleaner"]
}
```

After this, your keymap can add `SOCDON`, `SOCDOFF`, and `SOCDTOG` to any keymap position, as per the original blog post.
