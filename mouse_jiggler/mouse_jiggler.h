#pragma once

#include QMK_KEYBOARD_H
#include <stdint.h>
#include "deferred_exec.h"

/**
 * @brief Movement pattern identifiers.
 *
 * Pass one of these values to jiggler_set_pattern(), jiggler_set_pattern_intro(),
 * or jiggler_set_pattern_ending() to select a movement shape at runtime, or use
 * them as compile-time defaults for @c PD_JIGGLER_PATTERN, @c PD_JIGGLER_PATTERN_INTRO,
 * and @c PD_JIGGLER_PATTERN_ENDING.
 */
typedef enum pd_jiggler_pattern_t {
    PD_JIGGLER_PATTERN_SUBTLE         = 1, /**< Small, irregular random-delay jitter. */
    PD_JIGGLER_PATTERN_XLINE          = 2, /**< Horizontal (X-axis only) oscillation. */
    PD_JIGGLER_PATTERN_YLINE          = 3, /**< Vertical (Y-axis only) oscillation. */
    PD_JIGGLER_PATTERN_CIRCLE         = 4, /**< Clockwise circle, normal size. */
    PD_JIGGLER_PATTERN_CIRCLESMALL    = 5, /**< Clockwise circle, small size. */
    PD_JIGGLER_PATTERN_CIRCLECCW      = 6, /**< Counter-clockwise circle, normal size. */
    PD_JIGGLER_PATTERN_CIRCLECCWSMALL = 7, /**< Counter-clockwise circle, small size. */
    PD_JIGGLER_PATTERN_FIGURE         = 8, /**< Large figure-eight path. */
    PD_JIGGLER_PATTERN_SQUARE         = 9, /**< Square path. */
} pd_jiggler_pattern_t;

uint8_t jiggler_get_state(void);
void    jiggler_end(void);
void    jiggler_start(void);
void    jiggler_enable(void);
void    jiggler_disable(void);
void    jiggler_toggle(void);
void    jiggle_delay(uint32_t delay_sec);

bool     jiggler_get_autostop(void);
void     jiggler_set_autostop(bool enabled);
uint32_t jiggler_get_backoff(void);
void jiggler_set_backoff(uint32_t backoff_sec);

uint8_t jiggler_get_pattern(void);
uint8_t jiggler_get_pattern_intro(void);
uint8_t jiggler_get_pattern_ending(void);

void jiggler_set_pattern(uint8_t pattern);
void jiggler_set_pattern_intro(uint8_t pattern);
void jiggler_set_pattern_ending(uint8_t pattern);
void jiggler_pattern_next(void);
void jiggler_pattern_prev(void);
void jiggler_pattern_intro_next(void);
void jiggler_pattern_intro_prev(void);
void jiggler_pattern_ending_next(void);
void jiggler_pattern_ending_prev(void);

bool     jiggler_get_intro_enabled(void);
void     jiggler_set_intro_enabled(bool enabled);
uint32_t jiggler_get_intro_timeout(void);
void jiggler_set_intro_timeout(uint32_t timeout_ms);
