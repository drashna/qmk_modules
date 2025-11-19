# @drashna's QMK Community Modules

These rely on QMK Firmware 0.28.0 or later, merged in 2025q1.

In order to use these community modules with a build of QMK, this repo should be added to your external userspace as a submodule.

```sh
cd /path/to/your/external/userspace
git submodule add https://github.com/drashna/qmk_modules.git modules/drashna
git submodule update --init --recursive
```

Each child directory is a separate module, and has instructions on how to add it to your build.

## Debugging Modules

These are modules that add additional debug functionality.

| Module                                      | Description                                      |
| ------------------------------------------- | ------------------------------------------------ |
| [Console Keylogging](./console_keylogging/) | Adds the FAQ Debugging console code as a module. |
| [I2C Scanner](./i2c_scanner/)               | Adds an i2c scanner as a community module.       |

## Display Modules

These are modules that add additional functionality to displays (both OLED driver, and Quantum Painter)

| Module                          | Description                                                                                                     |
| ------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| [Bongocats](./bongocats)        | Gods, help me.                                                                                                  |
| [Display Menu](./display_menu/) | Adds support for @drashna's custom on-screen display code.                                                      |
| [QP Helpers](./qp_helpers/)     | Adds useful functions for Quantum Painter based displays. Mostly, this includes a simple line graph, currently. |

## Hardware Modules

These are modules that extend hardware functionality, such as extending (unused) hardware functionality or add drivers for new hardware.

| Module                  | Description                                                                       |
| ----------------------- | --------------------------------------------------------------------------------- |
| [RTC](./rtc/)           | Adds support for hardware RTC functionality (RP2040 built in RTC not working).    |
| [Watchdog](./watchdog/) | Adds hardware watchdog support as a community module. Additional config required. |

## Pointing Device Modules

These are modules that add additional functionality to displays (both OLED driver, and Quantum Painter)

| Module                                                  | Description                                                                                                 |
| ------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| [Drag Scroll](./drag_scroll/)                           | Adds Drag Scrolling support as a module.                                                                    |
| [Pointing Device Accel](./pointing_device_accel/)       | Adds maccel/pointing device acceleration support, ported directly from Burkfers and Wimad's implementation. |
| [Pointing Device Gestures](./pointing_device_gestures/) | Adds a module that allows triggering behavior based on which of the cardinal and ordinal direction moved.   |
| [Wiggle Ball](./wiggle_ball/)                           | Adds a module that triggers a behavior when wiggling the ball/mouse.                                        |

## Pointing Device Modules

These are modules that add additional functionality to displays (both OLED driver, and Quantum Painter)

| Module                              | Description                                                             |
| ----------------------------------- | ----------------------------------------------------------------------- |
| [Keyboard Lock](./keyboard_lock/)   | Adds a module that allows for disabling USB complete. Eg, a "cat mode". |
| [Layer Map](./layer_map/)           | Adds support for display rendering of active keyboard layout.           |
| [Unicode Typing](./unicode_typing/) | Adds support for communal unicode typing modes as a community module.   |

## RGB Modules

These are modules that add additional functionality to displays (both OLED driver, and Quantum Painter)

| Module                | Description                                                 |
| --------------------- | ----------------------------------------------------------- |
| [OpenRGB](./openrgb/) | Proof of concept for OpenRGB support as a community module. |

## In development

These modules are either not working properly yet (and are listed here for visibility) or are unsupported while waiting on PR merges/code changes. While it may be possible to rework these to function currently, .... eh.

| Module | Status | Description |
| ------ | ------ | ----------- |
| None   |        |             |
