# Pointing Device Smoothing

Smooths out jittery pointing device reports using an **Exponential Moving Average (EMA)**.

## How it works

Each axis is filtered independently:

$$\text{ema} = \alpha \cdot \text{raw} + (1 - \alpha) \cdot \text{ema}$$

- **High α (near 1.0)** — minimal smoothing, raw input passes through almost unchanged.
- **Low α (near 0.0)** — heavy smoothing, very lag-prone but very stable.

Sub-pixel precision is preserved via a rounding carry so slow movements don't get lost.  
The EMA state resets automatically after a configurable idle period so stale history doesn't affect the next burst of movement.

## Configuration

Compile-time defaults — add to your `config.h` to override:

```c
// Smoothing strength: 0.0 (max smooth) → 1.0 (no smooth). Default: 0.4
#define POINTING_DEVICE_SMOOTHING_FACTOR 0.4f

// Reset EMA state after this many ms of inactivity. Default: 200
#define POINTING_DEVICE_SMOOTHING_RESET_TIMEOUT_MS 200

// Per-keypress step for SM_FACT (Shift inverts, Ctrl ×10). Default: 0.05
#define POINTING_DEVICE_SMOOTHING_FACTOR_STEP 0.05f

// Per-keypress step in ms for SM_TIME (Shift inverts, Ctrl ×10). Default: 25
#define POINTING_DEVICE_SMOOTHING_TIMEOUT_STEP 25
```

## Keycodes

| Keycode | Alias | Description |
|---------|-------|-------------|
| `CM_SMOOTHING_TOGGLE` | `SM_TOGG` | Toggle smoothing on/off |
| `CM_SMOOTHING_FACTOR_INC` | `SM_FACT` | Increment smooth factor (+0.05). Hold **Shift** to decrement; hold **Ctrl** for ×10 step. |
| `CM_SMOOTHING_TIMEOUT_INC` | `SM_TIME` | Increment reset timeout (+25 ms). Hold **Shift** to decrement; hold **Ctrl** for ×10 step. |

## API

```c
// Enable / disable
bool pointing_device_smoothing_get_enabled(void);
void pointing_device_smoothing_set_enabled(bool enable);
void pointing_device_smoothing_toggle_enabled(void);

// Smooth factor  [0.0, 1.0]
float pointing_device_smoothing_get_factor(void);
void  pointing_device_smoothing_set_factor(float factor);   // clamped to [0.0, 1.0]
void  pointing_device_smoothing_factor_increment(void);     // respects held Shift / Ctrl

// Reset timeout  (milliseconds)
uint32_t pointing_device_smoothing_get_reset_timeout(void);
void     pointing_device_smoothing_set_reset_timeout(uint32_t timeout_ms);
void     pointing_device_smoothing_timeout_increment(void); // respects held Shift / Ctrl
```

## Usage in `qmk.json` / `modules` list

```json
{
  "modules": ["drashna/pointing_device_smoothing"]
}
```
