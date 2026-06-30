This adds rudimentary support for RGB Matrix based velocikey

It can be integrated into your keymap by adding the following to your `keymap.json`:

```json
{
    "modules": ["drashna/velocikey"]
}
```

## Functions

```c
void velocikey_toggle(void);
void velocikey_enable(void);
void velocikey_disable(void);
bool velocikey_get_enabled(void);
void velocikey_accelerate(void);
void velocikey_decelerate(void);
void velocikey_set_max_speed(uint8_t max_speed);
uint8_t velocikey_get_max_speed(void);
void velocikey_set_min_speed(uint8_t min_speed);
uint8_t velocikey_get_min_speed(void);
```
