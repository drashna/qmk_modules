# @drashna's QMK Community Modules

These rely on QMK Firmware 0.28.0 or later, merged in 2025q1.

In order to use these community modules with a build of QMK, this repo should be added to your external userspace as a submodule.

```sh
cd /path/to/your/external/userspace
git submodule add https://github.com/drashna/qmk_modules.git modules/drashna
git submodule update --init --recursive
```

Each child directory is a separate module, and has instructions on how to add it to your build.

| Module                                      | Description                                                                        |
|---------------------------------------------|------------------------------------------------------------------------------------|
| [Console Keylogging](./console_keylogging/) | Adds the FAQ Debugging console code as a module.                                   |
| [Display Menu](./display_menu/)             | Adds support for @drashna's custom on-screen display code.                         |
| [I2C Scanner](./i2c_scanner/)               | Adds an i2c scanner as a community module.                                         |
| [Keyboard Lock](./keyboard_lock/)           | Adds a module that allows for disabling USB complete. Eg, a "cat mode".            |
| [Layer Map](./layer_map/)                   | Adds support for display rendering of active keyboard layout.                      |
| [Unicode Typing](./unicode_typing/)         | Adds support for communal unicode typing modes as a community module.              |
| [Watchdog](./watchdog/)                     | Adds hardware watchdog support as a community module.  Additional config required. |
