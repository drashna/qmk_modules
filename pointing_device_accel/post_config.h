// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Enable float support for printf library
#ifdef POINTING_DEVICE_DEBUG
#    undef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#    define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1
#endif
