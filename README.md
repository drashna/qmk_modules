# @drashna's QMK Community Modules

These rely on QMK Firmware 0.28.0 or later, merged in 2025q1.

In order to use these community modules with a build of QMK, this repo should be added to your external userspace as a submodule.

```sh
cd /path/to/your/external/userspace
git submodule add https://github.com/drashna/qmk_modules.git modules/drashna
git submodule update --init --recursive
```

Each child directory is a separate module, and has instructions on how to add it to your build.

| Module                                    | Description                                                                        |
|-------------------------------------------|------------------------------------------------------------------------------------|
| [Custom Shift Keys](./custom_shift_keys/) | Adds support for getrueur's Custom Shift keys, as a community module               |
| [Globe Key](./globe_key/)                 | Adds a cut-down version of the macOS Globe Key to your build.                      |
| [I2C Scanner](./i2c_scanner/)             | Adds an i2c scanner as a community module.                                         |
| [Sentence Case](./sentence_case/)         | Adds support for @getrueur's Sentence Case, as a community module.                 |
| [SOCD Cleaner](./socd_cleaner/)           | Adds support for @getreuer's SOCD Cleaner, as a community module.                  |
| [Unicode Typing](./unicode_typing/)       | Adds support for communal unicode typing modes as a community module.              |
| [Watchdog](./watchdog/)                   | Adds hardware watchdog support as a community module.  Additional config required. |
