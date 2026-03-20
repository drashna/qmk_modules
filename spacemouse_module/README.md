# Spacemouse Module

Simplistic driver for adding support for Spacemouse Module. Specifically, this is for the UART version of the module.

This does not include pointing device driver support, as the multiple axises exceed what it can support,  and could probably use a bunch of edge case handling for the input.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/spacemouse_moudle"]
}
```

This doesn't do anything by itself (other than initialize the hardware and UART). Call `spacemouse_module_get_data()` to get a struct with x, y, z, twist, tilt x and tilt y values.

## Sample Pointing Device Implementation

This is a simple, very basic implementation of the hardware:

```c
bool use_tilt = false;

__attribute__((weak)) void spacemouse_module_handle_axes(spacemouse_data_t* spacemouse_data, report_mouse_t* mouse_report) {
    if (use_tilt) {
        mouse_report->x = CONSTRAIN_HID_XY(spacemouse_data->tilt_x);
        mouse_report->y = CONSTRAIN_HID_XY(spacemouse_data->tilt_y);
    } else {
        mouse_report->x = CONSTRAIN_HID_XY(spacemouse_data->x);
        mouse_report->y = CONSTRAIN_HID_XY(spacemouse_data->y);
    }
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (spacemouse_present) {
        spacemouse_data_t data = spacemouse_get_data();

        if (data.x || data.y || data.z || data.twist || data.tilt_x || data.tilt_y) {
            pd_dprintf("Raw ] X: %d, Y: %d, Z: %d, twist: %d, tilt X: %d, tilt Y: %d\n", data.x, data.y, data.z, data.twist, data.tilt_x, data.tilt_y);
        }
        spacemouse_module_handle_axes(&data, &mouse_report);
    }
    return mouse_report;
}
```

Additional functionality could be added, such as twisting being handled as an encoder action (even using the encoder driver), the Z axis being used a switch/push action, etc. 

## Notes

This could also be used as a gamepad, with the 6 axises supported by the device. And it could toggle between the two, if so inclined.

A lot can be done with this, but ... I leave that to you. 
