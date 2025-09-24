# QMK Typing Statistics Module v2.0

A comprehensive keyboard usage tracking module for QMK that collects detailed typing statistics and stores them persistently in EEPROM.

## Table of Contents

- [Overview](#overview)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Configuration](#configuration)
- [Public API Reference](#public-api-reference)
- [Optional Features](#optional-features)
- [Integration Examples](#integration-examples)
- [Migration Guide](#migration-guide)
- [Memory Usage](#memory-usage)

## Overview

This module tracks comprehensive typing statistics including:

- **WPM tracking**: Current, average (EMA), maximum, and session WPM
- **Key usage**: Per-position press counts with heatmap visualization
- **Layer analysis**: Usage counts and time spent on each layer
- **Modifier patterns**: Usage statistics for all 8 modifier keys
- **Hand balance**: Left/right hand usage distribution
- **Session tracking**: Separate statistics for current typing session
- **Bigram analysis**: Common key combinations (optional)
- **Advanced metrics**: Typing entropy, efficiency analysis

All data is automatically saved to EEPROM with configurable intervals and CRC protection.

### Version 2.0 Improvements

- **Clean Public API**: Clear separation between public and internal APIs
- **Modular Design**: Optional features are cleanly separated into packages
- **Better Documentation**: Comprehensive API documentation with usage examples
- **Backward Compatibility**: Existing code continues to work with minimal changes
- **Type Safety**: Improved type definitions and error handling

## Quick Start

```c
#include "typing_stats_public.h"

void keyboard_post_init_user(void) {
    ts_init();
}

void matrix_scan_user(void) {
    ts_task_10ms();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return ts_on_layer_change(state);
}

void eeconfig_init_user(void) {
    ts_eeconfig_init_user();
}

// Get statistics
ts_summary_t summary;
if (ts_get_summary(&summary)) {
    printf("WPM: %d, Total: %lu\n", summary.current_wpm, summary.total_lifetime_presses);
}
```

## Installation

### QMK Module Installation

Add the module to your `keymap.json`:

```json
{
    "modules": ["dmyoung9/typing_stats"],
    "config": {
        "features": {
            "console": true
        }
    }
}
```

Or if you prefer `rules.mk`:

```make
# Enable the typing stats module
QMK_MODULES += dmyoung9/typing_stats
CONSOLE_ENABLE = yes  # For debug output
```

### Minimal Integration

**keymap.c:**

```c
#include QMK_KEYBOARD_H
#include "typing_stats.h"  // Module header

void eeconfig_init_user(void) {
    ts_eeconfig_init_user();
}

void keyboard_post_init_user(void) {
    ts_init();
}

void matrix_scan_user(void) {
    ts_task_10ms();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return ts_on_layer_change(state);
}
```

That's it! The module handles all the implementation details automatically.

## Configuration

Add to your `config.h` to customize behavior:

```c
// Basic Configuration
#define TS_MAX_LAYERS 8          // Number of layers to track (default: 8)
#define TS_FLUSH_SECONDS 120     // Auto-save interval (default: 120s)
#define TS_FLUSH_EVENTS 2000     // Auto-save after N key presses (default: 2000)

// WPM Averaging (EMA smoothing factor)
#define TS_WPM_EMA_ALPHA_NUM 1   // Numerator (default: 1)
#define TS_WPM_EMA_ALPHA_DEN 8   // Denominator (default: 8) -> α = 1/8

// Advanced Features
#define TS_ENABLE_LAYER_TIME 1   // Track time spent on each layer
#define TS_ENABLE_BIGRAM_STATS 1 // Track common key combinations
```

## Public API Reference

The typing statistics module provides a clean, well-documented public API through `typing_stats_public.h`. All functions are prefixed with `ts_` and provide comprehensive error checking.

### System Management

#### `void ts_init(void)`

Initialize the typing statistics module. Must be called once during keyboard startup.

**Usage:**

```c
void keyboard_post_init_user(void) {
    ts_init();
}
```

#### `void ts_eeconfig_init_user(void)`

Initialize EEPROM storage for typing statistics. Call when EEPROM is reset.

**Usage:**

```c
void eeconfig_init_user(void) {
    ts_eeconfig_init_user();
}
```

### Event Processing

#### `void ts_task_10ms(void)`

Main task function for WPM sampling and auto-save. Call regularly (every scan cycle).

**Usage:**

```c
void matrix_scan_user(void) {
    ts_task_10ms();
}
```

#### `void ts_on_keyevent(keyrecord_t *record, uint16_t keycode)`

Process key press events. Feed all key events to this function.

**Parameters:**

- `record`: QMK keyrecord structure
- `keycode`: The keycode being processed

**Usage:**

```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);
    return true;
}
```

#### `layer_state_t ts_on_layer_change(layer_state_t new_state)`

Track layer state changes for layer usage statistics.

**Parameters:**

- `new_state`: The new layer state

**Returns:** The same layer state (pass-through)

**Usage:**

```c
layer_state_t layer_state_set_user(layer_state_t state) {
    return ts_on_layer_change(state);
}
```

### Statistics Query API

#### `bool ts_get_summary(ts_summary_t *summary)`

Get comprehensive statistics summary in a single call.

**Parameters:**

- `summary`: Pointer to summary structure to populate

**Returns:** `true` if successful, `false` if module not initialized

**Usage:**

```c
ts_summary_t summary;
if (ts_get_summary(&summary)) {
    printf("WPM: %d, Total: %lu, Left: %.1f%%\n",
           summary.current_wpm,
           summary.total_lifetime_presses,
           summary.left_hand_ratio * 100.0f);
}
```

#### Individual Statistics Functions

```c
uint16_t ts_get_current_wpm(void);        // Current WPM reading
uint16_t ts_get_avg_wpm(void);             // Average WPM (EMA)
uint16_t ts_get_max_wpm(void);             // Lifetime maximum WPM
uint16_t ts_get_session_max_wpm(void);     // Session maximum WPM
uint32_t ts_get_total_presses(void);       // Total lifetime presses
uint32_t ts_get_session_presses(void);     // Session presses
uint32_t ts_get_session_time_minutes(void); // Session duration
float    ts_get_left_hand_ratio(void);     // Left hand usage (0.0-1.0)
float    ts_get_right_hand_ratio(void);    // Right hand usage (0.0-1.0)
```

### Position Analysis API

#### `uint32_t ts_get_key_presses(uint8_t row, uint8_t col)`

Get press count for a specific key position.

**Usage:**

```c
uint32_t presses = ts_get_key_presses(2, 5);  // Get presses for row 2, col 5
```

#### `bool ts_find_most_used_key(uint8_t *row_out, uint8_t *col_out, uint32_t *count_out)`

Find the most frequently pressed key.

**Usage:**

```c
uint8_t row, col;
uint32_t count;
if (ts_find_most_used_key(&row, &col, &count)) {
    printf("Most used key: (%d,%d) with %lu presses\n", row, col, count);
}
```

### Layer Analysis API

#### `uint32_t ts_get_layer_presses(uint8_t layer)`

Get press count for a specific layer.

#### `bool ts_find_most_used_layer(uint8_t *layer_out, uint32_t *count_out)`

Find the most frequently used layer.

### Modifier Analysis API

#### `uint32_t ts_get_modifier_presses(uint8_t mod_index)`

Get press count for a specific modifier (0=LCtrl, 1=LShift, etc.).

#### `const char* ts_get_modifier_name(uint8_t mod_index)`

Get human-readable name for a modifier index.

### Session Management API

#### `void ts_start_new_session(void)`

Start a new typing session while preserving lifetime statistics.

**Usage:**

```c
ts_start_new_session();  // Reset session stats, keep lifetime data
```

**Example - Manual Session Control:**

```c
// In your keymap.c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);

    if (record->event.pressed) {
        switch (keycode) {
            case KC_F24:  // Custom key to start new session
                ts_start_new_session();
                return false;
        }
    }
    return true;
}
```

**What gets reset:**

- Session key presses → 0
- Session start time → current time
- Session maximum WPM → 0 (if WPM tracking enabled)

**What gets preserved:**

- Total lifetime presses
- Lifetime maximum WPM
- Average WPM (EMA)
- All key position statistics
- All layer and modifier statistics

#### `void ts_reset_all_stats(void)`

Reset all statistics to defaults. This action cannot be undone.

**Usage:**

```c
ts_reset_all_stats();  // Clear ALL data including lifetime stats
```

#### Automatic Session Management

By default, a new session starts automatically on each keyboard boot:

```c
#define TS_AUTO_NEW_SESSION_ON_BOOT 1  // Default: enabled
```

**When enabled (default):**

- New session starts every time the keyboard boots
- Lifetime statistics are preserved across reboots
- Session statistics reset to 0 on each boot

**When disabled:**

- Sessions continue across reboots
- Only starts new session on first-ever boot
- Use `ts_start_new_session()` for manual control

## Optional Features

The module includes several optional feature packages that can be enabled via configuration flags.

### Bigram Analysis (TS_ENABLE_BIGRAM_STATS)

Tracks common key sequences (bigrams) for typing pattern analysis.

```c
#define TS_ENABLE_BIGRAM_STATS 1
```

**API Functions:**

```c
bool ts_find_most_common_bigram(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out);
void ts_get_top_bigrams(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count);
void ts_reset_bigram_stats(void);
```

### WPM Tracking (TS_ENABLE_WPM_TRACKING)

Enables words-per-minute tracking and analysis. Includes current, average, and maximum WPM statistics.

```c
#define TS_ENABLE_WPM_TRACKING 1  // Default: enabled
```

**API Functions:**

```c
uint16_t ts_get_current_wpm(void);        // Current WPM reading
uint16_t ts_get_avg_wpm(void);             // Average WPM (exponential moving average)
uint16_t ts_get_max_wpm(void);             // Lifetime maximum WPM
uint16_t ts_get_session_max_wpm(void);     // Session maximum WPM
```

### Layer Time Tracking (TS_ENABLE_LAYER_TIME)

Tracks time spent on each layer in addition to key press counts.

```c
#define TS_ENABLE_LAYER_TIME 1
```

**API Functions:**

```c
uint32_t ts_get_layer_time_ms(uint8_t layer);     // Time in milliseconds
float ts_get_layer_time_ratio(uint8_t layer);     // Ratio of total time (0.0-1.0)
```

### EEPROM Storage (TS_ENABLE_EEPROM_STORAGE)

Enables persistent storage of statistics to EEPROM. When disabled, the module operates in memory-only mode.

```c
#define TS_ENABLE_EEPROM_STORAGE 1  // Default: enabled
```

**Memory-only mode benefits:**

- Reduced EEPROM wear
- Faster operation (no storage overhead)
- Suitable for temporary analysis or testing
- Statistics reset on each power cycle

### Advanced Analysis (TS_ENABLE_ADVANCED_ANALYSIS)

Provides advanced typing pattern analysis functions.

```c
#define TS_ENABLE_ADVANCED_ANALYSIS 1
```

**API Functions:**

```c
float ts_calculate_key_entropy(void);              // Key usage entropy
uint32_t ts_get_same_finger_presses(void);         // Same-finger sequences
uint32_t ts_get_finger_rolls(void);                // Finger roll sequences
float ts_calculate_hand_balance_score(void);       // Hand balance score (0.0-1.0)
float ts_calculate_finger_balance_score(void);     // Finger balance score
uint32_t ts_count_alternating_hands(void);         // Alternating hand sequences
float ts_calculate_typing_rhythm_variance(void);   // Rhythm consistency
```

## Integration Examples

### Basic OLED Display Integration

```c
#include "typing_stats_public.h"

void render_typing_stats(void) {
    ts_summary_t summary;
    if (!ts_get_summary(&summary)) return;

    oled_write_P(PSTR("Typing Stats:\n"), false);

    // Current session
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "WPM: %d\n", summary.current_wpm);
    oled_write(buffer, false);

    snprintf(buffer, sizeof(buffer), "Session: %lu\n", summary.session_presses);
    oled_write(buffer, false);

    // Lifetime stats
    snprintf(buffer, sizeof(buffer), "Total: %lu\n", summary.total_lifetime_presses);
    oled_write(buffer, false);

    snprintf(buffer, sizeof(buffer), "Max WPM: %d\n", summary.max_wpm);
    oled_write(buffer, false);

    // Hand balance
    snprintf(buffer, sizeof(buffer), "L/R: %.1f%%\n", summary.left_hand_ratio * 100.0f);
    oled_write(buffer, false);
}
```

### Advanced Analysis Example

```c
#if TS_ENABLE_ADVANCED_ANALYSIS
void show_advanced_stats(void) {
    float entropy = ts_calculate_key_entropy();
    float balance = ts_calculate_hand_balance_score();
    uint32_t same_finger = ts_get_same_finger_presses();
    uint32_t rolls = ts_get_finger_rolls();

    printf("Advanced Analysis:\n");
    printf("  Key Entropy: %.2f\n", entropy);
    printf("  Hand Balance: %.1f%%\n", balance * 100.0f);
    printf("  Same Finger: %lu\n", same_finger);
    printf("  Finger Rolls: %lu\n", rolls);
}
#endif
```

### Layer Usage Analysis

```c
void analyze_layer_usage(void) {
    printf("Layer Usage:\n");

    for (uint8_t layer = 0; layer < 4; layer++) {
        uint32_t presses = ts_get_layer_presses(layer);
        if (presses > 0) {
            printf("  Layer %d: %lu presses", layer, presses);

#if TS_ENABLE_LAYER_TIME
            uint32_t time_ms = ts_get_layer_time_ms(layer);
            float ratio = ts_get_layer_time_ratio(layer);
            printf(" (%.1fs, %.1f%%)", time_ms / 1000.0f, ratio * 100.0f);
#endif
            printf("\n");
        }
    }
}
```

## Migration Guide

### Migrating from v1.x to v2.0

Version 2.0 introduces a cleaner API structure while maintaining backward compatibility.

#### Header Changes

**Old (v1.x):**

```c
#include "typing_stats.h"  // Included everything
```

**New (v2.0):**

```c
#include "typing_stats_public.h"  // Clean public API
// OR for backward compatibility:
#include "typing_stats.h"         // Still works
```

#### Function Changes

Most functions remain the same, but some have been renamed for consistency:

| Old Function             | New Function           | Notes                               |
| ------------------------ | ---------------------- | ----------------------------------- |
| `ts_get_summary()`       | `ts_get_summary()`     | Now returns bool for error checking |
| `ts_start_new_session()` | `ts_reset_all_stats()` | More descriptive name               |
| Direct core access       | Use public API         | Internal functions are now private  |

#### New Features in v2.0

- **Error Handling**: Functions now return success/failure status
- **Type Safety**: Better type definitions and parameter validation
- **Modular Design**: Optional features are clearly separated
- **Documentation**: Comprehensive API documentation

#### Recommended Migration Steps

1. **Update includes**: Change to `typing_stats_public.h`
2. **Add error checking**: Check return values of new functions
3. **Update function calls**: Use new function names where applicable
4. **Enable features**: Explicitly enable optional features you need
5. **Test thoroughly**: Verify statistics are tracked correctly

````

## Query API

### Position Queries

#### `uint16_t ts_pos_to_index(uint8_t row, uint8_t col)`
Convert matrix position to linear index.

**Parameters:**
- `row`: Matrix row (0-based)
- `col`: Matrix column (0-based)

**Returns:** Linear index for position arrays

#### `void ts_index_to_pos(uint16_t index, uint8_t *row_out, uint8_t *col_out)`
Convert linear index back to matrix position.

**Parameters:**
- `index`: Linear index
- `row_out`: Output pointer for row (can be NULL)
- `col_out`: Output pointer for column (can be NULL)

#### `uint32_t ts_get_pos_presses(uint8_t row, uint8_t col)`
Get press count for specific key position.

**Parameters:**
- `row`: Matrix row
- `col`: Matrix column

**Returns:** Number of presses for this position

#### `bool ts_find_most_used_pos(uint16_t *index_out, uint32_t *count_out)`
Find the most frequently pressed key.

**Parameters:**
- `index_out`: Output pointer for position index (can be NULL)
- `count_out`: Output pointer for press count (can be NULL)

**Returns:** `true` if found, `false` if no data

#### `bool ts_find_least_used_pos(uint16_t *index_out, uint32_t *count_out, bool nonzero_only)`
Find the least frequently pressed key.

**Parameters:**
- `index_out`: Output pointer for position index (can be NULL)
- `count_out`: Output pointer for press count (can be NULL)
- `nonzero_only`: If true, ignore unused keys

**Returns:** `true` if found, `false` if no data

### Layer Queries

#### `uint32_t ts_get_layer_presses(uint8_t layer)`
Get press count for specific layer.

**Parameters:**
- `layer`: Layer number (0-based)

**Returns:** Number of presses on this layer

#### `int8_t ts_find_most_used_layer(uint8_t *layer_out, uint32_t *count_out)`
Find the most frequently used layer.

**Parameters:**
- `layer_out`: Output pointer for layer number (can be NULL)
- `count_out`: Output pointer for press count (can be NULL)

**Returns:** 1 if found, 0 if no data

#### `int8_t ts_find_least_used_layer(uint8_t *layer_out, uint32_t *count_out, bool nonzero_only)`
Find the least frequently used layer.

**Parameters:**
- `layer_out`: Output pointer for layer number (can be NULL)
- `count_out`: Output pointer for press count (can be NULL)
- `nonzero_only`: If true, ignore unused layers

**Returns:** 1 if found, 0 if no data

### Layer Time Tracking (if enabled)

#### `uint32_t ts_get_layer_time_ms(uint8_t layer)`
Get time spent on specific layer in milliseconds.

**Parameters:**
- `layer`: Layer number

**Returns:** Time in milliseconds

#### `float ts_get_layer_time_ratio(uint8_t layer)`
Get percentage of total time spent on specific layer.

**Parameters:**
- `layer`: Layer number

**Returns:** Time ratio (0.0-1.0)

### Modifier Queries

#### `uint32_t ts_get_mod_presses(uint8_t modbit_index)`
Get usage count for specific modifier.

**Parameters:**
- `modbit_index`: Modifier index (0-7: LCTL, LSFT, LALT, LGUI, RCTL, RSFT, RALT, RGUI)

**Returns:** Number of times modifier was used

#### `const char* ts_modbit_to_string(uint8_t modbit_index)`
Get human-readable name for modifier.

**Parameters:**
- `modbit_index`: Modifier index (0-7)

**Returns:** String name (e.g., "LCtrl", "RShift")

#### `int8_t ts_find_most_used_mod(uint8_t *modbit_out, uint32_t *count_out)`
Find the most frequently used modifier.

**Parameters:**
- `modbit_out`: Output pointer for modifier index (can be NULL)
- `count_out`: Output pointer for usage count (can be NULL)

**Returns:** 1 if found, 0 if no data

#### `int8_t ts_find_least_used_mod(uint8_t *modbit_out, uint32_t *count_out, bool nonzero_only)`
Find the least frequently used modifier.

**Parameters:**
- `modbit_out`: Output pointer for modifier index (can be NULL)
- `count_out`: Output pointer for usage count (can be NULL)
- `nonzero_only`: If true, ignore unused modifiers

**Returns:** 1 if found, 0 if no data

### Bigram Analysis (if enabled)

#### `bool ts_find_most_used_bigram(uint8_t *pos1_out, uint8_t *pos2_out, uint16_t *count_out)`
Find the most common key combination.

**Parameters:**
- `pos1_out`: Output pointer for first key position (can be NULL)
- `pos2_out`: Output pointer for second key position (can be NULL)
- `count_out`: Output pointer for combination count (can be NULL)

**Returns:** `true` if found, `false` if no data

#### `void ts_get_top_bigrams(ts_bigram_t *output, uint8_t max_count, uint8_t *actual_count)`
Get list of most common key combinations.

**Parameters:**
- `output`: Array to fill with bigram data
- `max_count`: Maximum number of bigrams to return
- `actual_count`: Output pointer for actual number returned (can be NULL)

## Debug & Maintenance

### Debug Output

#### `void ts_debug_print(void)`
Print basic statistics to console (requires `CONSOLE_ENABLE`).

**Usage:**
```c
ts_debug_print();  // Basic stats output
````

#### `void ts_debug_print_detailed(void)`

Print comprehensive statistics report to console.

**Usage:**

```c
ts_debug_print_detailed();  // Full analysis
```

#### `void ts_debug_print_heatmap(void)`

Print visual heatmap of key usage to console.

**Usage:**

```c
ts_debug_print_heatmap();  // Visual key usage map
```

### Data Management

#### `void ts_mark_dirty(void)`

Mark statistics as changed to trigger save on next flush cycle.

#### `void ts_force_flush(void)`

Immediately save all statistics to EEPROM.

**Usage:**

```c
ts_force_flush();  // Save now
```

#### `void ts_reset_defaults(void)`

Reset all statistics to default values (in RAM only).

**Usage:**

```c
ts_reset_defaults();  // Clear all stats
ts_force_flush();     // Make permanent
```

### Advanced Analysis

#### `float ts_calculate_key_distribution_entropy(void)`

Calculate Shannon entropy of key usage distribution.

**Returns:** Entropy in bits (higher = more even distribution)

#### `uint32_t ts_estimate_consecutive_same_finger(void)`

Get estimated count of consecutive same-finger presses.

**Returns:** Rough estimate count

#### `uint32_t ts_estimate_finger_rolls(void)`

Get estimated count of finger roll sequences.

**Returns:** Rough estimate count

## OLED Integration

### Display Functions

#### `void ts_render_oled_stats(void)`

Render basic statistics to OLED display.

**Usage:**

```c
#ifdef OLED_ENABLE
bool oled_task_user(void) {
    ts_render_oled_stats();
    return false;
}
#endif
```

#### `void ts_render_oled_heatmap(void)`

Render key usage heatmap to OLED display.

## Data Structures

### `ts_summary_t`

Comprehensive statistics summary structure.

```c
typedef struct {
    uint32_t total_lifetime_presses;  // Total keys pressed ever
    uint32_t session_presses;         // Keys pressed this session
    uint16_t current_wpm;             // Current WPM reading
    uint16_t avg_wpm;                 // Average WPM (EMA)
    uint16_t max_wpm;                 // Lifetime maximum WPM
    uint16_t session_max_wpm;         // Session maximum WPM
    float    left_hand_ratio;         // Left hand usage (0.0-1.0)
    uint8_t  most_used_layer;         // Most frequently used layer
    uint8_t  most_used_mod;           // Most frequently used modifier
    uint16_t most_used_pos_index;     // Most frequently used key position
} ts_summary_t;
```

### `ts_bigram_t` (if enabled)

Key combination tracking structure.

```c
typedef struct {
    uint8_t  key1_pos;  // Position index of first key
    uint8_t  key2_pos;  // Position index of second key
    uint16_t count;     // Frequency of this combination
} ts_bigram_t;
```

### `ts_hand_t`

Hand identification enumeration.

```c
typedef enum {
    TS_HAND_LEFT,     // Left hand key
    TS_HAND_RIGHT,    // Right hand key
    TS_HAND_UNKNOWN   // Cannot determine
} ts_hand_t;
```

## Examples

### Basic Usage

```c
// Get current typing stats
uint16_t current_wpm = ts_get_current_wpm();
uint16_t avg_wpm = ts_get_avg_wpm();
uint32_t total_keys = ts_get_total_presses();

printf("WPM: %u (avg: %u), Total keys: %lu\n",
       current_wpm, avg_wpm, total_keys);
```

### Find Problem Areas

```c
// Check hand balance
float left_ratio = ts_get_left_hand_ratio();
if (left_ratio < 0.4 || left_ratio > 0.6) {
    printf("Hand balance issue: %.1f%% left\n", left_ratio * 100);
}

// Find unused keys
uint16_t unused_pos;
uint32_t unused_count;
if (ts_find_least_used_pos(&unused_pos, &unused_count, false)) {
    if (unused_count == 0) {
        uint8_t row, col;
        ts_index_to_pos(unused_pos, &row, &col);
        printf("Unused key at (%u,%u)\n", row, col);
    }
}
```

### Layer Analysis

```c
// Analyze layer usage patterns
for (uint8_t i = 0; i < TS_MAX_LAYERS; i++) {
    uint32_t presses = ts_get_layer_presses(i);
    if (presses > 0) {
        printf("Layer %u: %lu presses", i, presses);

        #if TS_ENABLE_LAYER_TIME
        float time_ratio = ts_get_layer_time_ratio(i);
        printf(" (%.1f%% time)", time_ratio * 100);
        #endif

        printf("\n");
    }
}
```

### Custom Keycode Integration

```c
enum custom_keycodes {
    STATS_PRINT = SAFE_RANGE,
    STATS_RESET,
    NEW_SESSION
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    ts_on_keyevent(record, keycode);  // Always feed to stats

    if (record->event.pressed) {
        switch (keycode) {
            case STATS_PRINT:
                ts_debug_print_detailed();
                return false;

            case STATS_RESET:
                ts_reset_defaults();
                ts_force_flush();
                return false;

            case NEW_SESSION:
                ts_start_new_session();
                return false;
        }
    }

    return true;
}
```

## Memory Usage

### RAM Usage

- **Base module**: ~50 bytes static variables
- **Per-position tracking**: 2 bytes × matrix size (e.g., 96 bytes for 6×8 matrix)
- **Layer tracking**: 4 bytes × `TS_MAX_LAYERS`
- **Modifier tracking**: 32 bytes
- **Session data**: ~20 bytes
- **Bigram tracking** (optional): ~200 bytes

**Total RAM**: ~150-400 bytes depending on matrix size and features

### EEPROM Usage

- **Base data**: ~200 bytes
- **Per-position data**: 2 bytes × matrix size
- **Layer time tracking**: +4 bytes × `TS_MAX_LAYERS`
- **Bigram data**: +~200 bytes

**Total EEPROM**: ~300-600 bytes depending on matrix size and features

### Flash Usage

- **Core implementation**: ~3-4KB
- **OLED support**: +~1KB
- **Advanced features**: +~1KB

**Total Flash**: ~4-6KB

## Performance Notes

- All query functions are O(n) over their respective arrays
- Statistics update is O(1) per keypress
- Auto-save triggers are optimized to minimize EEPROM wear
- WPM sampling occurs every 50ms maximum
- CRC calculation is performed only during save operations

---

_For support and updates, refer to the QMK community resources and typing_stats module documentation._
