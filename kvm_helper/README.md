# KVM Helper

This module improves USB stability when using a keyboard through a KVM switch or when switching between multiple hosts. Some KVMs cause the keyboard to get stuck during USB re-enumeration (remaining in the init state rather than reaching configured), which prevents normal operation. This module detects those situations and performs a soft reset to recover.

When OS Detection with `OS_DETECTION_KEYBOARD_RESET` is already enabled, this module doesn't do anything, as this module is based directly on the OS Detection reset code.  No need to have two paths to the same functionality.

## Configuration

| Define                    | Default | Description                                                                    |
|---------------------------|---------|--------------------------------------------------------------------------------|
| `KVM_DETECTION_DEBOUNCE`  | `250`   | Time in milliseconds to wait before acting on a USB state change. Max `2000`. |

## Usage

Add the module to your `keymap.json`:

```json
{
    "modules": ["drashna/kvm_helper"]
}
```
