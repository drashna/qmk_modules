// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @brief Because this is a stupid idea, why not.   Adds support for watchdog monitoring of keyboard.
 *
 */

#include "quantum.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#ifdef __arm__
#    include <hal.h>
#    include <math.h>
#    include <stddef.h>

#    if defined(MCU_STM32)
// STM32-specific watchdog config calculations
// timeout = tick * PR * (RL + 1)
// tick = 1000000 / (lsi) clock
#        if !defined(WATCHDOG_CLOCK)
#            define WATCHDOG_CLOCK STM32_LSICLK
#        endif // !defined(WATCHDOG_CLOCK)

#        define _IWDG_LSI(us)    ((us) * WATCHDOG_CLOCK / 1000000)
#        define _IWDG_PR_US(us)  (uint8_t)(log(_IWDG_LSI(us)) / log(2) - 11) // 3
#        define _IWDG_PR_S(s)    _IWDG_PR_US(s * 1000000.0)
#        define _IWDG_SCALAR(us) (2 << ((uint8_t)_IWDG_PR_US(us) + 1))
#        define _IWDG_RL_US(us)  (uint64_t)(_IWDG_LSI(us)) / _IWDG_SCALAR(us)
#        define _IWDG_RL_S(s)    _IWDG_RL_US(s * 1000000.0)
#    elif defined(MCU_RP)
// investigate the actual timing this uses
#        define _IWDG_RL_S(s) (uint32_t)(s * 1000)
#    else
#        error "Current MCU not supported yet"
#    endif

#    if !defined(WATCHDOG_TIMEOUT)
#        define WATCHDOG_TIMEOUT 5.0f
#    endif // !defined(WATCHDOG_TIMEOUT)

static WDGConfig wdgcfg = {
#    if STM32_HAS_IWDG == TRUE
    .pr = _IWDG_PR_S(WATCHDOG_TIMEOUT),
#    endif // STM32_HAS_IWDG
    .rlr = _IWDG_RL_S(WATCHDOG_TIMEOUT),
#    if STM32_IWDG_IS_WINDOWED == TRUE
    .winr = STM32_IWDG_WIN_DISABLED,
#    endif // STM32_IWDG_IS_WINDOWED
};

void keyboard_post_init_watchdog(void) {
    wdgInit();

    wdgStart(&WDGD1, &wdgcfg);
    keyboard_post_init_watchdog_kb();
}

void housekeeping_task_watchdog(void) {
    wdgReset(&WDGD1);
    housekeeping_task_watchdog_kb();
}

bool shutdown_watchdog(bool jump_to_bootloader) {
    wdgStop(&WDGD1);
    return shutdown_watchdog_kb(jump_to_bootloader);
}

void suspend_power_down_watchdog(void) {
    wdgReset(&WDGD1);
    suspend_power_down_watchdog_kb();
}

void suspend_wakeup_init_watchdog(void) {
    wdgReset(&WDGD1);
    suspend_wakeup_init_watchdog_kb();
}

#elif defined(__AVR__)
#    error AVR Watchdog causes issues, likely due to the way the matrix scan is done.  Disable this.
#endif // __AVR__
