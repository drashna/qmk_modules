# Bongocats

This adds the simple bongocat animation to the board to the oled. Intended for a 128x32 oled.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/bongocats"]
}
```

This also sets up the `oled_task_user(void)` function to render the animation, but is defined "weakly" so you can use it without any additional setup, or handle it more manually.

The actual processing is handled by `void render_bongocats(void)`, so you can throw `render_bongocats()` in your `oled_task_user` function with additional code.

## Configuration

| Setting                             | Default | Description                                                                             |
| ----------------------------------- | ------- | --------------------------------------------------------------------------------------- |
| `BONGOCATS_IDLE_FRAMES`             | `5`     | How many idle frames to use.                                                            |
| `BONGOCATS_IDLE_SPEED`              | `10`    | Update interval in ms for idle animation.                                               |
| `BONGOCATS_PREP_FRAMES`             | `1`     | How many frames between transition between idle and tapping.                            |
| `BONGOCATS_TAP_FRAMES`              | `2`     | How many frames for tapping animation.                                                  |
| `BONGOCATS_ANIM_WPM_LOWER`          | `20`    | Lower limit of WPM to start animiating at.                                              |
| `BONGOCATS_ANIM_FRAME_DURATION_MAX` | `450`   | How long to keep a single frame displayed, while tapping                                |
| `BONGOCATS_ANIM_FRAME_DURATION_MIN` | `100`   | Shortest time before switching frame (eg, upper limit of speed for tapping animiation). |
| `BONGOCATS_IDLE_FRAME_DURATION`     | `300`   | Interval between updating frames for idle animation.                                    |
| `BONGOCATS_ANIM_FRAME_RATIO`        | `2.5`   | How aggressively to speed up tapping animation speed with WPM.                          |
