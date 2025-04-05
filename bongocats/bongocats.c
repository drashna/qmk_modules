// Copyright 2022 HorrorTroll <https://github.com/HorrorTroll>
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPD-License-Identifier: GPL-2.0-or-later

#include <quantum.h>
#include <oled_driver.h>
#include "bongocats.h"

static uint32_t curr_anim_duration = 0; // variable animation duration
static uint32_t bongo_timer        = 0;
static uint8_t  current_idle_frame = 0;
static uint8_t  current_tap_frame  = 0;
#if BONGOCATS_PREP_FRAMES > 1
static uint8_t current_prep_frame = 0;
#endif

// Code containing pixel art, contains:
// 5 idle frames, 1 prep frame, and 2 tap frames
extern const char PROGMEM bongocats_idle[BONGOCATS_IDLE_FRAMES][BONGOCATS_ANIM_SIZE];
extern const char PROGMEM bongocats_tap[BONGOCATS_TAP_FRAMES][BONGOCATS_ANIM_SIZE];
#if BONGOCATS_PREP_FRAMES > 0
extern const char PROGMEM bongocats_prep[BONGOCATS_PREP_FRAMES][BONGOCATS_ANIM_SIZE];
#endif
// assumes 1 frame prep stage
static void animation_phase(void) {
    if (get_current_wpm() <= BONGOCATS_IDLE_SPEED) {
        current_idle_frame = (current_idle_frame + 1) % BONGOCATS_IDLE_FRAMES;
        oled_write_raw_P(bongocats_idle[abs((BONGOCATS_IDLE_FRAMES - 1) - current_idle_frame)], BONGOCATS_ANIM_SIZE);
    }

    if (get_current_wpm() > BONGOCATS_IDLE_SPEED && get_current_wpm() < BONGOCATS_ANIM_WPM_LOWER) {
#if BONGOCATS_PREP_FRAMES > 0
#    if BONGOCATS_PREP_FRAMES > 1
        oled_write_raw_P(bongocats_prep[abs((BONGOCATS_PREP_FRAMES - 1) - current_prep_frame)],
                         BONGOCATS_ANIM_SIZE); // uncomment if IDLE_FRAMES >1
#    endif
        oled_write_raw_P(bongocats_prep[0], BONGOCATS_ANIM_SIZE); // remove if IDLE_FRAMES >1
#endif
    }

    if (get_current_wpm() >= BONGOCATS_ANIM_WPM_LOWER) {
        current_tap_frame = (current_tap_frame + 1) % BONGOCATS_TAP_FRAMES;
        oled_write_raw_P(bongocats_tap[abs((BONGOCATS_TAP_FRAMES - 1) - current_tap_frame)], BONGOCATS_ANIM_SIZE);
    }
}

void render_bongocat(void) {
#if OLED_TIMEOUT > 0
    if (last_input_activity_elapsed() > OLED_TIMEOUT) {
        return;
    }
#endif

    // variable animation duration. Don't want this value to get near zero as it'll bug out.
    curr_anim_duration = MAX(BONGOCATS_ANIM_FRAME_DURATION_MIN,
                             BONGOCATS_ANIM_FRAME_DURATION_MAX - BONGOCATS_ANIM_FRAME_RATIO * get_current_wpm());

    if (get_current_wpm() > BONGOCATS_ANIM_WPM_LOWER) {
        if (timer_elapsed32(bongo_timer) > curr_anim_duration) {
            bongo_timer = timer_read32();
            animation_phase();
        }

    } else {
#if OLED_TIMEOUT > 0
        if (last_input_activity_elapsed() < OLED_TIMEOUT)
#endif
        {
            if (timer_elapsed32(bongo_timer) > BONGOCATS_IDLE_FRAME_DURATION) {
                bongo_timer = timer_read32();
                animation_phase();
            }
        }
    }
}

__attribute__((weak)) bool oled_task_user(void) {
    render_bongocat();
    return false;
}
