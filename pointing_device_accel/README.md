This adds the Pointing Device feature Acceleration code as a community module.

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/pointing_device_accel"]
}
```

This enables extended mouse reports, as well as float printing, automatically.  Floats are used, but shouldn't significantly impact performance.  However, the extended mouse reports are enabled so that it reduces the likelihood that the pointing device reports will get maxed out by the acceleration. (+/- 127 vs +/- 32,767)

Note that a majority of this readme is taken straight from [here](https://github.com/burkfers/qmk_userspace_features/blob/main/maccel/readme.md).   Massive credit goes to Wimads (@wimads) and burkfers (@burkfers), whom wrote the code.

## Caveats

If using without PR25050 merged, then you need to add this to your config.h:

```c
#define POINTING_DEVICE_ACCEL_SHIM
```

And you'll need to add `pointing_device_task_pointing_device_accel` to your `keymap.c`.  Eg:

```c
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    mouse_report = pointing_device_task_pointing_device_accel(mouse_report);
    return mouse_report;
}

## Configuration

Before configuring maccel, make sure you have turned off your OS acceleration settings: On Windows, this setting is called "Enhance pointer precision". And make sure there isn't any 3rd party mouse acceleration software running.

Several characteristics of the acceleration curve can be tweaked by adding relevant defines to `config.h`:

```c
#define POINTING_DEVICE_ACCEL_TAKEOFF 2.0      // lower/higher value = curve takes off more smoothly/abruptly
#define POINTING_DEVICE_ACCEL_GROWTH_RATE 0.25 // lower/higher value = curve reaches its upper limit slower/faster 
#define POINTING_DEVICE_ACCEL_OFFSET 2.2       // lower/higher value = acceleration kicks in earlier/later
#define POINTING_DEVICE_ACCEL_LIMIT 0.2        // lower limit of accel curve (minimum acceleration factor)
```

Or can be set at runtime with these functions:

* `pointing_device_accel_set_takeoff(float val)`
* `pointing_device_accel_set_growth_rate(float val)`
* `pointing_device_accel_set_offset(float val)`
* `pointing_device_accel_set_limit(float val)`

The graph below shows the acceleration curve. You can interpret this graph as follows: the horizontal axis is input velocity (ie. how fast you are physically moving your mouse/trackball/trackpad); the vertical axis is the acceleration factor, which is the factor with which the input velocity will be multiplied, resulting in your new output velocity on screen. You can also understand this as a DPI scaling factor: the curve maxes out at 1, meaning your mouse sensitivity will never go higher than your default DPI setting; at the start of the curve your sensitivity is scaled down to a minimum that can be set by the LIMIT variable. The limit in this example is 0.2, which means at the lowest velocity your mouse sensitivity is scaled down to an equivalent of 0.2 times your default DPI.

[![](assets/accel_curve.png)](https://www.desmos.com/calculator/k9vr0y2gev)

**If you click on the image of the curve**, you will be linked to Desmos, where you can play around with the variables to understand how each of them affect the shape of the curve. But in short:

* The TAKEOFF variable controls how smoothly or abruptly the acceleration curve takes off. A higher value will make it take off more abruptly, a lower value smoothens out the start of the curve.

* The GROWTH_RATE variable sets the growth rate of the acceleration curve. A lower value will result in a flatter curve which takes longer to reach its LIMIT. A higher value will result in a steeper curve, which will reach its LIMIT faster.

* The OFFSET variable moves the entire curve towards the right. Offsetting the curve to the right means acceleration will kick in later, which is useful for low speed precision - in effect what you would otherwise have used SNIPING mode for. The maccel feature basically eliminates the need for a sniping mode.

* The LIMIT variable sets the lower limit for the acceleration curve. This is the minimum acceleration factor at which the curve will start. In effect this adjusts the sensitivity for low speed precision movements.

* The upper limit of the curve is fixed at 1, which means that at high speed sensitivity equals your default DPI. If you want to adjust high speed sensitivity, adjust your DPI.

A good starting point for tweaking your settings, is to set your default DPI slightly higher than what you'd use without acceleration. Then set your LIMIT variable to a factor that would scale down to what you normally might have set your sniping DPI. For example, if your usual default DPI is 900, you might set it now to 1000. And if your usual sniping DPI is 200, you might set your LIMIT to 0.2 (0.2*1000=200). From there you can start playing around with the variables until you arrive at something to your liking.

**Debug console**: To aid in dialing in your settings just right, a debug mode exists to print mathy details to the console. The debug console will print your current DPI setting and variable settings, as well as the acceleration factor, the input and output velocity, and the input and output distance. Refer to the QMK documentation on how to [*enable the console and debugging*](https://docs.qmk.fm/#/faq_debug?id=debugging), then enable mouse acceleration debugging by adding the following defines in `config.h`:

```c
#define POINTING_DEVICE_DEBUG
```

## Runtime adjusting of curve parameters by keycodes (optional)


To use keycodes to adjust the parameters without recompiling, two more build steps are required.
First, add five keycodes to your keycode enum. You may choose different names, as long as you use the same names in the following step. If you are not yet using custom keycodes, add the following snippet to `keymap.c`:


| Keycode          |Short code | Description                                                 |
|------------------|-----------|-------------------------------------------------------------|
| `MA_TOGGLE`      | `MA_TOGG` | Toggle acceleration.                                        |
| `MA_TAKEOFF`     | `MA_TKOF` | Acceleration curve takeoff (initial acceleration) step key. |
| `MA_GROWTH_RATE` | `MA_GROW` | Acceleration curve growth rate step key.                    |
| `MA_OFFSET`      | `MA_OFST` | Acceleration curve offset step key.                         |
| `MA_LIMIT`       | `MA_LMT`  | Acceleration curve limit step key.                          |

## Acceleration keycode usage

The four keycodes can be used to adjust the curve parameters. This is *not* persisted unless you also enabled the via option or add your own provisioning for persistence - Adjusted values are printed to the console to aid in finding the right settings for `config.h`. The step keys will adjust the parameters by the following amounts, which can optionally be adjusted:

| Parameter    | Default step value | Define name                         |
| ---          | ---                | ---                                 |
| Takeoff      | `+0.01`            | `POINTING_DEVICE_ACCEL_TAKEOFF`     |
| Growth rate  | `+0.01`            | `POINTING_DEVICE_ACCEL_GROWTH_RATE` |
| Offset       | `+0.1`             | `POINTING_DEVICE_ACCEL_OFFSET`      |
| Limit        | `+0.01`            | `POINTING_DEVICE_ACCEL_LIMIT`       |

The modifier keys can be used to alter the step effect:

| Modifier    | Effect                                    |
| ---         | ---                                       |
| Shift       | Reverse step (subtract instead of adding) |
| Control     | `step value * 10` (Step 10 times faster)  |

Modifiers can be combined.

With every adjustment, an informational message is printed to the console.

## VIA support (optional)

Mouse acceleration can now be configured though via.

![](assets/via.png)
\* *note that VIA unfortunately cannot show the numeric values for the sliders, so you will still need to use the debug console to monitor any changes you make to the variables. (The numbers shown in the image have just been edited in to indicate the range of the sliders).*

### Additional installation steps for adding VIA support:
If your keyboard is not already supported by via, you must first [create a via definition](https://www.caniusevia.com/docs/specification).

Please be aware of the following caveats:
- The via support takes over the "via custom config" block. If you are already storing values in eeprom in your userspace, you must manually merge the features.
- The  via support implements `via_custom_value_command_kb`. This is not compatible with keyboards that already add custom features to via. If your keyboard has custom via configuration, you must manually shim the keyboard-level callback.

Create a custom via definition: Find your keyboard's via definition in the [via keyboards repository](https://github.com/the-via/keyboards/tree/master/v3) if you did not create your own.

Extend its `menus` configuration by placing the [menu definition](assets/via.json) on the `menu` node.

Finally, after flashing the firmware to your board, load the custom via definition in the design tab in [via](https://usevia.app) (you may have to enable the "Show Design Tab" option in the settings).

## Limitations

**Mac OS compatibility:**

*It's complicated...* Yes, you can use acceleration with MacOS, but there is some caveats to consider:

* Even after disabling the OS level acceleration, macOS does some kind of post processing to smooth cursor movement, which you cannot disable. This will distort the results, which will make it harder to dial in, and the result may or may not be satisfactory.
* Secondly, the OS level acceleration of macOS seems to be quite good, so whether acceleration (with the smoothing issue in mind) will actually be an improvement is debatable. We've heard opinions both ways from macOS users. So in other words, your mileage may vary.

**Sensor compatibility:**

* PMW3360: fully compatible, elaborately tested
* Other PMW33xx sensors will very likely perform equally well (but not tested so far)
* adns5050: compatible, tested
* Cirque trackpad: compatible, limited testing
* Azoteq: not compatible, due to issues in how the driver handles DPI settings.
* No other QMK compatible sensors have been tested so far. We expect most sensors to work fine with acceleration, but there could always be unexpected driver/firmware related conflicts we are not aware of.
* If you are using acceleration successfully (or unsuccessfully) with a sensor that isn't listed here, we'd love to hear!

**MCU compatibility:**
Despite our initial worries about the extensive use of floating point operations likely not working well on AVR, it's been tested and works adequately. However, firmware size might be an issue. Depending on what other QMK features you have enabled, you may need to make some compromises to fit maccel.

It is currently unknown how the un-throttled polling when used with `POINTING_DEVICE_MOTION_PIN` would interact with the expensive calculations.

## Breaking changes

### 2024 March 12

This new release changes the acceleration curve from a up-scaling curve to a down-scaling curve, to match how other acceleration tools work, and to avoid forcing users to set a very low DPI setting - this had been the goal from the start, but it took until now to overcome the technical challenges to make this work smoothly.

See the configuration bit of this readme for an explanation of how the new curve works. This change means that you will have to readjust your variables; but do not worry, it is fairly easy to get this dialed in to *exactly* to how you had it set before:

* First, change your default DPI: $DPI_{new} = DPI_{old} * {limit}_{old}$
* Second, change your LIMIT variable (which is now lower instead of upper limit): $limit_{new} = \dfrac{DPI_{old}}{DPI_{new}}$
* Your other variables can remain the same.
* If using via, make sure to clear EEPROM for the new settings to take effect.

### 2024 March 10

A keycode for toggling mouse acceleration was added: If you enabled maccel keycodes, you must add a fifth keycode to your enum and add it to the shim between the record and takeoff arguments. Don't forget to place it on your keymap!

### 2024 March 1

If you're updating from a previous version, you will have to make manual adjustments to your integration. Refer to the instructions for details on what the current version expects:
- The parameters have changed names and were expanded:
  - `STEEPNESS` is now `GROWTH_RATE`, `TAKEOFF` was added
    - Change the define names in your `config.h` for the parameter settings and step settings (if applicable)
    - If using keycodes: Change the shim in `process_record_user` to call a fourth keycode
- If using via: Clear EEPROM and use a new via json. Do NOT load a previous via backup without adjusting the maccel values to the new format!

If you set GROWTH_RATE to your previous value of `STEEPNESS` and keep `TAKEOFF` at a high value (eg. `10`), the behavior will be similar to previous versions.

## Release history
- 2024 March 12 - Release of improved down scaling accel curve
- 2024 March 10 - Addition of toggle keycode
- 2024 March 1 - Release of new four-parameter acceleration curve
- 2024 February 23 - New four-parameter acceleration curve and improved documentation
- 2024 February 07 - Experimental new DPI correction to achieve consistent acceleration behavior across different user DPI settings.
- 2024 February 06 - First release candidate. Feedback welcome!

## Credits
Thanks to everyone who helped!
Including, but not limited to:
- Wimads (@wimads) and burkfers (@burkfers) wrote most of the code
- ankostis (@ankostis) for catalyzing discussion about improving the acceleration curve and providing several enhancements
- Quentin (@balanstik) for insightful commentary on the math, and testing
- ouglop (@ouglop) for insightful commentary on the math
- Drashna Jael're (@drashna) for coding tips and their invaluable bag of magic C tricks
- bcl (@energetic_beagle_99245) for testing on AVR with an adns5050 sensor
