# OpenRGB

Blarg, adds openRGB support as a community module. This mostly works, with a few caveats (see below).

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/openrgb"]
}
```

## Caveat #1 - USB Endpoint Sizing

Unfortunately, OpenRGB requires that the raw endpoint bo 64 bytes (rather than the standard 32), and this isn't noramlly configurable.

The module automatically patches `tmk_core/protocol/usb_descriptor.h` at build time — replacing the bare `#define RAW_EPSIZE 32` with a guarded version so the endpoint size can be overridden:

```c
#ifndef RAW_EPSIZE
#    define RAW_EPSIZE 32
#endif
```

No manual file edits are required. 

## Caveat #2 - RGB Matrix Animations

OpenRGB hard codes the animations that are supported, so some of the newer animations are not available.  

## Caveat #3 - Support

This module is not an endorsement of OpenRGB in any sense. If there are issues with this code, whatever. I may attempt to fix it, but that may or may not happen. This is mostly a proof of concept/viablility, and to be blunt the openRGB folks should:

1. fork/copy this code
2. maintain/support this themselves
