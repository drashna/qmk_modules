#pragma message "RTC not supported on AVR chips"
#include "vendor.h"

void vendor_rtc_set_time(rtc_time_t time) {}
void vendor_rtc_get_time(rtc_time_t *time) {
    time->year            = 0;
    time->month           = 0;
    time->day_of_the_week = 0;
    time->date            = 0;
    time->second          = 0;
    time->minute          = 0;
    time->hour            = 0;
    time->am_pm           = false;
    time->is_dst          = false;
}

bool vendor_rtc_init(rtc_time_t *time) {
    return false;
}

bool vendor_rtc_task(rtc_time_t *time) {
    return false;
}
