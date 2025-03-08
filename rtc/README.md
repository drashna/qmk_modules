# RTC (Real-time Clock)

This enables real time clock functionality, to be able to set the date and time locally in the firmware and keep it synced.

Add the following to the list of modules in your `keymap.json` to enable this module:

```json
{
    "modules": ["drashna/rtc"]
}
```


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
