# OpenRGB

Blarg, adds openRGB support as a community module. This mostly works, with a few caveats (see below).

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/openrgb"]
}
```

## Caveat #1 - USB Endpoint Sizing

Unfortunatately, somebody decided that 64 Bytes was the best idea for openRGB's protocol. We can't change that in openRGB, but we can change that locally. In fact, we must modify QMK firmware code to be able to do this. In `qmk_firmware/tmk_core/protocol/usb_descriptor.h`, at the end of the file is a list of the endpoint sizes, Find the `#define RAW_EPSIZE 32` and replace that with this block:

```c
#ifndef RAW_EPSIZE
#    define RAW_EPSIZE 32
#endif
```

Save the file (and commit it), and then you should be able to compile with no issues.

## Caveat #2 - RGB Matrix Animations

Unfortately, the openRGB side code is so very out of date, and was never very flexible in dealing with animations. XAP has a number of handlings that will make this better, but that's neither here, nor there.

The animation order is no longer correct, and since I don't use openRGB, I can't be fucked to figure out the mapping and fix it. If you have a correct mapping that works, and want to submit that as a PR here, feel free.

## Caveat #3 - Support

I am not supporting OpenRGB in any sense. If there are issues with this code, whatever. I may attempt to fix it, but ... I don't have any plans on really doing that, especially long term. This is mostly a proof of concept/viablility, and to be blunt the openRGB folks should:

1. fork/copy this code and maintain it themselves
2. fix the endpoint to use 32 bytes
3. maintain/support this themselves
