// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#undef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1

#define EECONFIG_MODULE_RTC_DATA_SIZE    1
#define SPLIT_TRANSACTION_IDS_MODULE_RTC RPC_ID_RTC_TIME_SYNC
