// Copyright 2026 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#ifndef DISPLAY_KEYLOGGER_LENGTH
#    define DISPLAY_KEYLOGGER_LENGTH 25
#endif // DISPLAY_KEYLOGGER_LENGTH

#define EECONFIG_MODULE_DISPLAY_KEYLOGGER_DATA_SIZE ((DISPLAY_KEYLOGGER_LENGTH + 1) * 4)
#define SPLIT_TRANSACTION_IDS_MODULE_DISPLAY_KEYLOGGER RPC_ID_DISPLAY_KEYLOGGER_SYNC
