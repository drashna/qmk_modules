/* Copyright 2022 HorrorTroll <https://github.com/HorrorTroll>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <quantum.h>
#include <stdlib.h>
#include <oled_driver.h>

// WPM-responsive animation stuff here
#ifndef IDLE_FRAMES
#    define IDLE_FRAMES 5
#endif // IDLE_FRAMES
// below this wpm value your animation will idle
#ifndef IDLE_SPEED
#    define IDLE_SPEED 10
#endif // IDLE_SPEED
#ifndef PREP_FRAMES
#    define PREP_FRAMES 1
#endif // PREP_FRAMES
#ifndef TAP_FRAMES
#    define TAP_FRAMES 2
#endif // TAP_FRAMES
// above this wpm value typing animation to trigger
#ifndef ANIM_WPM_LOWER
#    define ANIM_WPM_LOWER 20
#endif // ANIM_WPM_LOWER
// longest animation duration in ms
#ifndef ANIM_FRAME_DURATION_MAX
#    define ANIM_FRAME_DURATION_MAX 450
#endif // ANIM_FRAME_DURATION_MAX
// shortest animation duration in ms
#ifndef ANIM_FRAME_DURATION_MIN
#    define ANIM_FRAME_DURATION_MIN 100
#endif // ANIM_FRAME_DURATION_MIN
// how long each frame lasts in ms
#ifndef IDLE_FRAME_DURATION
#    define IDLE_FRAME_DURATION 300
#endif // IDLE_FRAME_DURATION
// how aggressively animation speeds up with wpm
#ifndef ANIM_FRAME_RATIO
#    define ANIM_FRAME_RATIO 2.5
#endif // ANIM_FRAME_RATIO

#define ANIM_SIZE 512 // number of bytes in array, minimize for adequate firmware size, max is 1024

uint32_t curr_anim_duration = 0; // variable animation duration
uint32_t bongo_timer        = 0;
uint32_t bongo_sleep        = 0;
uint8_t  current_idle_frame = 0;
uint8_t  current_prep_frame = 0;
uint8_t  current_tap_frame  = 0;

// Code containing pixel art, contains:
// 5 idle frames, 1 prep frame, and 2 tap frames
#include "bongocat_assets.h"

// assumes 1 frame prep stage
static void animation_phase(void) {
    if (get_current_wpm() <= IDLE_SPEED) {
        current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
        oled_write_raw_P(idle[abs((IDLE_FRAMES - 1) - current_idle_frame)], ANIM_SIZE);
    }

    if (get_current_wpm() > IDLE_SPEED && get_current_wpm() < ANIM_WPM_LOWER) {
#if PREP_FRAMES > 0
#    if IDLE_FRAMES > 1
        oled_write_raw_P(prep[abs((PREP_FRAMES - 1) - current_prep_frame)], ANIM_SIZE); // uncomment if IDLE_FRAMES >1
#    endif
        oled_write_raw_P(prep[0], ANIM_SIZE); // remove if IDLE_FRAMES >1
#endif
    }

    if (get_current_wpm() >= ANIM_WPM_LOWER) {
        current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
        oled_write_raw_P(tap[abs((TAP_FRAMES - 1) - current_tap_frame)], ANIM_SIZE);
    }
}

void render_bongocat(void) {
#if OLED_TIMEOUT > 0
    if (timer_elapsed32(bongo_sleep) > OLED_TIMEOUT) {
        return;
    }
#endif

    // variable animation duration. Don't want this value to get near zero as it'll bug out.
    curr_anim_duration = MAX(ANIM_FRAME_DURATION_MIN, ANIM_FRAME_DURATION_MAX - ANIM_FRAME_RATIO * get_current_wpm());

    if (get_current_wpm() > ANIM_WPM_LOWER) {
        if (timer_elapsed32(bongo_timer) > curr_anim_duration) {
            bongo_timer = timer_read32();
            animation_phase();
        }

        bongo_sleep = timer_read32();
    } else {
#if OLED_TIMEOUT > 0
        if (timer_elapsed32(bongo_sleep) < OLED_TIMEOUT)
#endif
        {
            if (timer_elapsed32(bongo_timer) > IDLE_FRAME_DURATION) {
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
