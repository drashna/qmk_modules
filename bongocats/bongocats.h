// Copyright 2022 HorrorTroll <https://github.com/HorrorTroll>
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPD-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdlib.h>
#include <progmem.h>

// WPM-responsive animation stuff here
#ifndef BONGOCATS_IDLE_FRAMES
#    define BONGOCATS_IDLE_FRAMES 5
#endif // BONGOCATS_IDLE_FRAMES
// below this wpm value your animation will idle
#ifndef BONGOCATS_IDLE_SPEED
#    define BONGOCATS_IDLE_SPEED 10
#endif // BONGOCATS_IDLE_SPEED
#ifndef BONGOCATS_PREP_FRAMES
#    define BONGOCATS_PREP_FRAMES 1
#endif // BONGOCATS_PREP_FRAMES
#ifndef BONGOCATS_TAP_FRAMES
#    define BONGOCATS_TAP_FRAMES 2
#endif // BONGOCATS_TAP_FRAMES
// above this wpm value typing animation to trigger
#ifndef BONGOCATS_ANIM_WPM_LOWER
#    define BONGOCATS_ANIM_WPM_LOWER 20
#endif // BONGOCATS_ANIM_WPM_LOWER
// longest animation duration in ms
#ifndef BONGOCATS_ANIM_FRAME_DURATION_MAX
#    define BONGOCATS_ANIM_FRAME_DURATION_MAX 450
#endif // BONGOCATS_ANIM_FRAME_DURATION_MAX
// shortest animation duration in ms
#ifndef BONGOCATS_ANIM_FRAME_DURATION_MIN
#    define BONGOCATS_ANIM_FRAME_DURATION_MIN 100
#endif // BONGOCATS_ANIM_FRAME_DURATION_MIN
// how long each frame lasts in ms
#ifndef BONGOCATS_IDLE_FRAME_DURATION
#    define BONGOCATS_IDLE_FRAME_DURATION 300
#endif // BONGOCATS_IDLE_FRAME_DURATION
// how aggressively animation speeds up with wpm
#ifndef BONGOCATS_ANIM_FRAME_RATIO
#    define BONGOCATS_ANIM_FRAME_RATIO 2.5
#endif // BONGOCATS_ANIM_FRAME_RATIO

// for 128x32 OLEDs, this should be 512 (128*32 = 4096 bits = 512 bytes)
#define BONGOCATS_ANIM_SIZE 512

extern const char PROGMEM bongocats_idle[BONGOCATS_IDLE_FRAMES][BONGOCATS_ANIM_SIZE];
extern const char PROGMEM bongocats_tap[BONGOCATS_TAP_FRAMES][BONGOCATS_ANIM_SIZE];
#if BONGOCATS_PREP_FRAMES > 0
extern const char PROGMEM bongocats_prep[BONGOCATS_PREP_FRAMES][BONGOCATS_ANIM_SIZE];
#endif

void render_bongocats(void);
