# Tractyl Pointer Config

Provides the DPI/sniper-mode pointer configuration behavior originally implemented in `keyboards/handwired/tractyl_manuform/tractyl_manuform.c` (itself derived from `keyboards/bastardkb/charybdis`), as a reusable Community Module.

Handles:

* Persisting default/sniping DPI and the split-transaction sync of that
  state between keyboard halves, via its own module EEPROM datablock
  (`EECONFIG_MODULE_TRACTYL_CONFIG_DATA_SIZE`).
* The `POINTER_DEFAULT_DPI_FORWARD`/`REVERSE`, `POINTER_SNIPING_DPI_FORWARD`/`REVERSE`,
  `SNIPING_MODE`, and `SNIPING_MODE_TOGGLE` keycodes.

## Usage

Add to `keymap.json`:

```json
{
    "modules": ["drashna/tractyl"]
}
```

Optionally configure via:

```c
#define TRACTYL_MINIMUM_DEFAULT_DPI     400
#define TRACTYL_DEFAULT_DPI_CONFIG_STEP 200
#define TRACTYL_MINIMUM_SNIPING_DPI     200
#define TRACTYL_SNIPING_DPI_CONFIG_STEP 100
```
