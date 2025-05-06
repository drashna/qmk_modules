# RTC (Real-time Clock)

This enables real time clock functionality, to be able to set the date and time locally in the firmware and keep it synced.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/rtc"]
}
```

If a time has not been set for the RTC, then the compile date and time are used for the initial initialization.  This won't be completely accurate, but should only be off by a matter of seconds, in most cases.  And should be good enough to get you going.

## Keycodes

| Keycode               | Description                                            |
|-----------------------|--------------------------------------------------------|
| `RTC_YEAR_INCREASE`   | Increments the year by one.                            |
| `RTC_YEAR_DECREASE`   | Decrements the year by one.                            |
| `RTC_MONTH_INCREASE`  | Increments the month by one.                           |
| `RTC_MONTH_DECREASE`  | Decrements the month by one.                           |
| `RTC_DATE_INCREASE`   | Increments the date by one.                            |
| `RTC_DATE_DECREASE`   | Decrements the data by one.                            |
| `RTC_HOUR_INCREASE`   | Increments the hour by one.                            |
| `RTC_HOUR_DECREASE`   | Decrements the hour by one.                            |
| `RTC_MINUTE_INCREASE` | Increments the minute by one.                          |
| `RTC_MINUTE_DECREASE` | Decrements the minute by one.                          |
| `RTC_SECOND_INCREASE` | Increments the second by one.                          |
| `RTC_SECOND_DECREASE` | Decrements the second by one.                          |
| `RTC_AM_PM_TOGGLE`    | Toggles between AM and PM when in 12 Hour time format. |
| `RTC_FORMAT_TOGGLE`   | Toggles between 12 and 24 hour format.                 |
| `RTC_DST_TOGGLE`      | Toggles DST setting.                                   |

## Functions

| Function                                      | Description                                                                                       |
|-----------------------------------------------|---------------------------------------------------------------------------------------------------|
| `rtc_time_t rtc_read_time_struct(void)`       | Reads the current time, via the `rtc_time_t` structure.                                           |
| `bool rtc_is_connected(void)`                 | Checks if RTC is connected and working.                                                           |
| `void rtc_set_time(rtc_time_t time)`          | Sets the current time for the RTC.                                                                |
| `bool bool rtc_set_time_kb(rtc_time_t *time)` | Keyboard callback for when the time is being set, if keyboard code needs to make any adjustments. |
| `bool rtc_set_time_user(rtc_time_t *time)`    | User callback for when the time is being set, if the user code needs to make any adjustments.     |

## String Report Functions

All of these functions return a pre-formatted date/time in a string. Great for OLED/Quantum Painter display, or console logging.

| Function                           | Sample Value                                                           |
|------------------------------------|------------------------------------------------------------------------|
| `rtc_read_date_str()`              | `12/30/2024`                                                           |
| `rtc_read_time_str()`              | `02:32:12PM` or `13:16:54`                                             |
| `rtc_read_date_time_str()`         | `12/30/2024 02:32:12PM` (eg, the previous two combined).               |
| `rtc_read_date_time_iso8601_str()` | `2024-12-30T02:32:12PM` (a close approximation if the ISO 8601 format) |

## Supported Hardware

By default, "vendor" is the selected RTC hardware.  This requires hardware support for the RTC and may require hardware config.  And obviously, requires a battery to be hooked up to the appropriate pins.

| Driver    | Description                                                                                                               | Links |
|:---------:|---------------------------------------------------------------------------------------------------------------------------|-------|
| `vendor`  | This allows for build in hardware support of the RTC functionality. AVR is not supported, and RP2040 is currently bugged. |       |
| `ds3231`  | A low cost and high precision I2C based RTC module with integrated temperature compensation. Supports 5V and 3.3V.        | [Adafruit](https://www.adafruit.com/product/5188) |
| `ds1307`  | A low cost I2C based RTC module.  May lose up to 2 seconds a day. "Requires" 5V power.                                    | [Adafruit](https://www.adafruit.com/product/3296)  |
| `pcf8523` | A super low cost I2C based RTC module.  May lose up to 2 seconds a day.  Supports 5V and 3.3V.                            | [Adafruit](https://www.adafruit.com/product/5189) |

### Vendor Hardware

As mentioned above, currently, RP2040 is not supported by the vendor hardware due to a bug in the RTC driver code.  And I'm not sure where it is (it looks to match the pico-SDK code, which does apparently work fine).

For STM32 based chips, you will need to check your schematic and data sheet to see if RTC suppport is available, and how to support it.  Ideally, you should have an LSE crystal, and the RTC should be configured to use it.  This will get you the most reliable functionality, from what I understand.

As an example, the [STM32F405 Tractyl Manuform](https://github.com/qmk/qmk_firmware/blob/e3c613c79c2211163abb914c8725cb495942fbb9/keyboards/handwired/tractyl_manuform/5x6_right/f405/mcuconf.h#L21-L28) has RTC enabled at the keyboard level, for instance.

```c
#undef STM32_LSE_ENABLED
#define STM32_LSE_ENABLED TRUE

#undef STM32_RTCSEL
#define STM32_RTCSEL STM32_RTCSEL_LSE
```

Also, to ensure that the keyboard keeps time after power loss, you will need to ensure a bettery is hooked up (a simple coin cell battery is more than enough for this), and that the controller has the correct circuitry to support this.  WeAct's Blackpill (STM32F4x1) and CoreBoard (STM32F405) have all the required components and I have personally verified that they do work.
