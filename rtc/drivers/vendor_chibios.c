// Copyright 2022 Jose Pablo Ramirez <jp.ramangulo@gmail.com>
// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vendor.h"
#include <hal.h>
#include "debug.h"
#include <math.h>
#include "print.h"
#include "timer.h"
#include <stdio.h>

// RP2040 RTC is busted?
// time comes back as 8077251600 (0x1E1711410)
// aka 2225/12/16 09:00:00

void convert_halrtc_to_local_rtc_struct(RTCDateTime *halrtc, rtc_time_t *local) {
    local->year            = halrtc->year + 1980U;
    local->month           = halrtc->month;
    local->day_of_the_week = halrtc->dayofweek;
    local->date            = halrtc->day;
    local->second          = (halrtc->millisecond / 1000) % 60;
    local->minute          = (halrtc->millisecond / (1000 * 60)) % 60;
    local->format          = RTC_FORMAT_24H;
    if (local->format == RTC_FORMAT_12H) {
        local->hour = (halrtc->millisecond / (1000 * 60 * 60)) % 12;
    } else {
        local->hour = (halrtc->millisecond / (1000 * 60 * 60)) % 24;
    }
    local->am_pm = (halrtc->millisecond / (1000 * 60 * 60)) <= 12;

    local->is_dst = (bool)halrtc->dstflag;

    local->unixtime = convert_to_unixtime(*local);
}

void convert_local_rtc_to_halrtc_struct(rtc_time_t *local, RTCDateTime *halrtc) {
    halrtc->year        = local->year - 1980U;
    halrtc->month       = local->month;
    halrtc->dayofweek   = local->day_of_the_week;
    halrtc->day         = local->date;
    halrtc->millisecond = (local->second + (local->minute * 60) + (local->hour * 60 * 60)) * 1000;
    halrtc->dstflag     = (uint32_t)local->is_dst;
}

void vendor_rtc_set_time(rtc_time_t time) {
    RTCDateTime timespec = {0};
    convert_local_rtc_to_halrtc_struct(&time, &timespec);
    rtcSetTime(&RTCD1, &timespec);
}

void vendor_rtc_get_time(rtc_time_t *time) {
    RTCDateTime timespec = {0};
    rtcGetTime(&RTCD1, &timespec);
    time->is_dst = (bool)timespec.dstflag;
    convert_halrtc_to_local_rtc_struct(&timespec, time);
}

bool vendor_rtc_init(rtc_time_t *time) {
    vendor_rtc_get_time(time);

#ifdef RTC_FORCE_INIT
    if (true)
#else
    if (time->year < 2000)
#endif // RTC_FORCE_INIT
    {
        dprintf("Vendor RTC: Date/time not set. Setting to compiled date/time as fallback!\n");
        *time = convert_timestamp(__TIMESTAMP__);
        rtc_check_dst_format(time);

        vendor_rtc_set_time(*time);
    } else {
        dprintf("vendor RTC: Initialized and initial read performed\n");
    }
    return true;
}

bool vendor_rtc_task(rtc_time_t *time) {
    vendor_rtc_get_time(time);
    return true;
}
