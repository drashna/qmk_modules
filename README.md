# @drashna's QMK Community Modules

These rely on QMK Firmware 0.28.0 or later, merged in 2025q1.

In order to use these community modules with a build of QMK, this repo should be added to your external userspace as a submodule.

```sh
cd /path/to/your/external/userspace
git submodule add https://github.com/drashna/qmk_modules.git modules/drashna
git submodule update --init --recursive
```

Each child directory is a separate module, and has instructions on how to add it to your build.

| Module                          | Description                                                       |
|---------------------------------|-------------------------------------------------------------------|
| [Globe Key](./globe_key/)       | Adds a cut-down version of the macOS Globe Key to your build.     |
| [SOCD Cleaner](./socd_cleaner/) | Adds support for @getreuer's SOCD Cleaner, as a community module. |
