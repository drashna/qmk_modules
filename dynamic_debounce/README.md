# Dynamic Debounce

Wraps every stock QMK debounce algorithm (`quantum/debounce/*.c`) behind a single
dispatcher, so the active algorithm and debounce time can be changed at runtime
instead of being fixed at compile-time via `DEBOUNCE_TYPE`/`DEBOUNCE`. Settings
are persisted to EEPROM and, on split keyboards, synced from the master to the
other half.

Available algorithms (`dynamic_debounce_algo_t`):

- `DYNAMIC_DEBOUNCE_SYM_DEFER_G` (default) — equivalent to `sym_defer_g`
- `DYNAMIC_DEBOUNCE_SYM_EAGER_PR` — equivalent to `sym_eager_pr`
- `DYNAMIC_DEBOUNCE_SYM_DEFER_PR` — equivalent to `sym_defer_pr`
- `DYNAMIC_DEBOUNCE_SYM_EAGER_PK` — equivalent to `sym_eager_pk`
- `DYNAMIC_DEBOUNCE_SYM_DEFER_PK` — equivalent to `sym_defer_pk`
- `DYNAMIC_DEBOUNCE_ASYM_EAGER_DEFER_PK` — equivalent to `asym_eager_defer_pk`
- `DYNAMIC_DEBOUNCE_NONE` — no debouncing

If the debounce time is set to `0`, the `none` algorithm is always used
regardless of the selected algorithm.

## Usage

Add the module to your `keymap.json`:

```json
{
    "modules": ["drashna/dynamic_debounce"]
}
```

The module registers itself as the keyboard's debounce implementation (setting
`DEBOUNCE_TYPE = custom` internally), so don't set `DEBOUNCE_TYPE` yourself.
The regular `DEBOUNCE` define (default `5`) only controls the initial debounce
time in milliseconds, used the first time the keyboard boots with no saved
EEPROM data.

### Keycodes

| Keycode                         | Alias     | Description                     |
|---------------------------------|-----------|---------------------------------|
| `CM_DYNAMIC_DEBOUNCE_NEXT`      | `DD_NEXT` | Cycle to the next algorithm     |
| `CM_DYNAMIC_DEBOUNCE_PREV`      | `DD_PREV` | Cycle to the previous algorithm |
| `CM_DYNAMIC_DEBOUNCE_TIME_UP`   | `DD_TUP`  | Increase the debounce time      |
| `CM_DYNAMIC_DEBOUNCE_TIME_DOWN` | `DD_TDN`  | Decrease the debounce time      |

The time step used by `DD_TUP`/`DD_TDN` defaults to `1` (ms) and can be changed
with `DYNAMIC_DEBOUNCE_TIME_STEP`.

### API

```c
#include "dynamic_debounce.h"

dynamic_debounce_algo_t dynamic_debounce_get_algorithm(void);
void                    dynamic_debounce_set_algorithm(dynamic_debounce_algo_t algo);
void                    dynamic_debounce_next_algorithm(void);
void                    dynamic_debounce_previous_algorithm(void);
const char             *dynamic_debounce_get_algorithm_name(dynamic_debounce_algo_t algo);

uint8_t dynamic_debounce_get_time(void);
void    dynamic_debounce_set_time(uint8_t time_ms);
void    dynamic_debounce_increase_time(uint8_t step);
void    dynamic_debounce_decrease_time(uint8_t step);
```

Set a different default at compile-time with:

```c
#define DYNAMIC_DEBOUNCE_DEFAULT_ALGO DYNAMIC_DEBOUNCE_SYM_EAGER_PK
```

## Persistence

The selected algorithm and debounce time are stored in EEPROM and restored on
boot. Writes are debounced to at most once every
`DYNAMIC_DEBOUNCE_EECONFIG_FLUSH_MS` (default `500`) to avoid wearing out the
EEPROM when rapidly cycling settings.

## Split keyboard support

On split keyboards, the master half periodically pushes its current algorithm
and debounce time to the other half (throttled by `FORCED_SYNC_THROTTLE_MS`,
default `100`), so both halves always debounce with the same settings — only
the master's EEPROM copy is authoritative.
