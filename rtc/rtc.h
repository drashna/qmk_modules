// Copyright 2022 Jose Pablo Ramirez <jp.ramangulo@gmail.com>
// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RTC_AM = 0x00, /**< am */
    RTC_PM = 0x01, /**< pm */
} rtc_time_am_pm_t;

typedef enum {
    RTC_FORMAT_12H = 0x01, /**< 12h format */
    RTC_FORMAT_24H = 0x00, /**< 24h format */
} rtc_time_format_t;

typedef enum {
    RTC_SUNDAY    = 0x00, /**< sunday */
    RTC_MONDAY    = 0x01, /**< monday */
    RTC_TUESDAY   = 0x02, /**< tuesday */
    RTC_WEDNESDAY = 0x03, /**< wednesday */
    RTC_THURSDAY  = 0x04, /**< thursday */
    RTC_FRIDAY    = 0x05, /**< friday */
    RTC_SATURDAY  = 0x06, /**< saturday */
} rtc_time_day_of_the_week_t;

typedef struct {
    uint16_t                   year;            /**< year */
    uint8_t                    month;           /**< month */
    uint8_t                    date;            /**< date */
    rtc_time_day_of_the_week_t day_of_the_week; /**< week */
    uint8_t                    hour;            /**< hour */
    uint8_t                    minute;          /**< minute */
    uint8_t                    second;          /**< second */
    rtc_time_format_t          format;          /**< data format */
    rtc_time_am_pm_t           am_pm;           /**< am pm */
    bool                       is_dst;          /**< daylight saving time */
    int8_t                     timezone;        /**< timezone */
    uint32_t                   unixtime;        /**< Unix Time */
} rtc_time_t;

#define SECONDS_FROM_1970_TO_2000 946684800

uint32_t   convert_to_unixtime(rtc_time_t time);
rtc_time_t convert_date_time(const char *date, const char *time);
rtc_time_t convert_timestamp(const char *timestamp);

uint8_t rtc_bcd2bin(uint8_t val);
uint8_t rtc_bin2bcd(uint8_t val);
uint8_t week_to_int(uint8_t d);

rtc_time_t rtc_read_time_struct(void);
char      *rtc_read_date_str(void);
char      *rtc_read_time_str(void);
char      *rtc_read_date_time_str(void);
char      *rtc_read_date_time_iso8601_str(void);

bool     rtc_is_connected(void);
void     rtc_set_time_split(rtc_time_t time, bool is_connected);
uint32_t get_fattime(void);

void rtc_set_time(rtc_time_t time);
bool rtc_set_time_kb(rtc_time_t *time);
bool rtc_set_time_user(rtc_time_t *time);
void rtc_check_dst_format(rtc_time_t *time);

void rtc_year_increase(void);
void rtc_year_decrease(void);
void rtc_month_increase(void);
void rtc_month_decrease(void);
void rtc_date_increase(void);
void rtc_date_decrease(void);
void rtc_hour_increase(void);
void rtc_hour_decrease(void);
void rtc_minute_increase(void);
void rtc_minute_decrease(void);
void rtc_second_increase(void);
void rtc_second_decrease(void);
void rtc_format_toggle(void);
void rtc_am_pm_toggle(void);
void rtc_dst_toggle(void);
