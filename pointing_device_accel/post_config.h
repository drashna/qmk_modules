// Copyright 2024 ZSA Technology Labs, Inc <@zsa>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Enable float support for printf library
#ifdef POINTING_DEVICE_DEBUG
#undef PRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PRINTF_SUPPORT_DECIMAL_SPECIFIERS 1
#endif

// Crude hack to enable eeprom storage for accel config and prevent overlap with start of VIA eeprom storage
#if defined(VIA_ENABLE)
#    if !defined(VIA_EEPROM_CUSTOM_CONFIG_SIZE)
#        define VIA_EEPROM_CUSTOM_CONFIG_SIZE (17)
#    else
#        if defined(VIA_EEPROM_CUSTOM_CONFIG_ERROR_OVERRIDE)
#            pragma message \
                "VIA_EEPROM_CUSTOM_CONFIG_SIZE already defined, make sure custom functions are handling eeprom."
#        else
#            error "VIA_EEPROM_CUSTOM_CONFIG_SIZE already defined. See docs for more information."
#        endif // VIA_EEPROM_CUSTOM_CONFIG_ERROR_OVERRIDE
#    endif     // VIA_EEPROM_CUSTOM_CONFIG_SIZE
#endif         // VIA_ENABLE
