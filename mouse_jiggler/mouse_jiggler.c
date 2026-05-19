/**
 * @file mouse_jiggler.c
 * @brief Mouse jiggler implementation for QMK.
 *
 * Periodically sends small mouse movements via deferred execution to prevent
 * the host from going idle. Supports multiple movement patterns (circle,
 * square, subtle, etc.) and optional intro/outro sequences.
 *
 * Cannibalised from:
 * https://getreuer.info/posts/keyboards/macros3/index.html#a-mouse-jiggler
 */

#include QMK_KEYBOARD_H
#include <stdbool.h>
#include "mouse_jiggler.h"
#include "pointing_device_internal.h"

/**
 * @brief Default intro/outro animation timeout in milliseconds.
 *
 * Used to initialise @c jiggler_intro_timeout at startup.  Override in
 * @c config.h before the header is included.  Set @c PD_JIGGLER_NOINTRO to
 * have the runtime variable start at 0 (timeout disabled) regardless.
 */
#if !defined(PD_JIGGLER_INTRO_TIMEOUT)
#    define PD_JIGGLER_INTRO_TIMEOUT 1000
#endif // PD_JIGGLER_INTRO_TIMEOUT

#if !defined(PD_JIGGLER_PATTERN)
#    define PD_JIGGLER_PATTERN PD_JIGGLER_PATTERN_SUBTLE
#endif // defined(PD_JIGGLER_PATTERN)

#if !defined(PD_JIGGLER_PATTERN_INTRO)
#    define PD_JIGGLER_PATTERN_INTRO PD_JIGGLER_PATTERN_CIRCLESMALL
#endif // defined(PD_JIGGLER_PATTERN_INTRO)
#if !defined(PD_JIGGLER_PATTERN_ENDING)
#    define PD_JIGGLER_PATTERN_ENDING PD_JIGGLER_PATTERN_CIRCLECCWSMALL
#endif // defined(PD_JIGGLER_PATTERN_ENDING)

/**
 * @brief Default backoff delay in seconds after any real user input.
 *
 * Used to initialise @c jiggler_backoff at startup.  Override in @c config.h.
 * The value can be changed at runtime via jiggler_set_backoff().
 */
#if !defined(PD_JIGGLER_BACKOFF)
#    define PD_JIGGLER_BACKOFF 30
#endif // defined(PD_JIGGLER_BACKOFF)

report_mouse_t msJigReport          = {0};
deferred_token msJigMainToken       = INVALID_DEFERRED_TOKEN;
deferred_token msJigIntroToken      = INVALID_DEFERRED_TOKEN;
deferred_token msJigIntroTimerToken = INVALID_DEFERRED_TOKEN;

static uint8_t jiggler_active_pattern = PD_JIGGLER_PATTERN;
static uint8_t jiggler_active_intro   = PD_JIGGLER_PATTERN_INTRO;
static uint8_t jiggler_active_ending  = PD_JIGGLER_PATTERN_ENDING;

/**
 * @brief Whether intro/outro animations are enabled at runtime.
 *
 * Initialised to @c false when @c PD_JIGGLER_NOINTRO is defined at compile
 * time, @c true otherwise.  Can be toggled at runtime via
 * jiggler_set_intro_enabled().
 */
#if defined(PD_JIGGLER_NOINTRO)
static bool jiggler_intro_enabled = false;
#else
static bool jiggler_intro_enabled = true;
#endif

/**
 * @brief Intro/outro animation timeout in milliseconds.
 *
 * When the timer expires jiggler_introtimer() cancels the intro/outro.
 * Set to 0 to disable the timeout entirely.  Initialised from
 * @c PD_JIGGLER_INTRO_TIMEOUT.
 */
static uint32_t jiggler_intro_timeout = PD_JIGGLER_INTRO_TIMEOUT;

/**
 * @brief Backoff delay in seconds applied after any real user input.
 *
 * Whenever a key or pointing-device event is detected the next jiggler tick
 * is pushed forward by this many seconds, preventing simulated movement from
 * colliding with real input.  Initialised from @c PD_JIGGLER_BACKOFF.
 */
static uint32_t jiggler_backoff = PD_JIGGLER_BACKOFF;

/**
 * @brief Whether the jiggler automatically stops on any keypress.
 *
 * Initialised to @c true when @c PD_JIGGLER_AUTOSTOP is defined at compile
 * time, @c false otherwise.  Can be changed at runtime via
 * jiggler_set_autostop().
 */
#if defined(PD_JIGGLER_AUTOSTOP)
static bool jiggler_autostop = true;
#else
static bool jiggler_autostop = false;
#endif

/**
 * @brief Returns the current jiggler state.
 *
 * @return 0 if the jiggler is inactive,
 *         1 if the main jiggler loop is running,
 *         2 if the intro/outro pattern is also running.
 */
uint8_t jiggler_get_state(void) {
    if (msJigMainToken != INVALID_DEFERRED_TOKEN) {
        if (msJigIntroToken != INVALID_DEFERRED_TOKEN) {
            return 2;
        } else {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Core jiggler tick: advances one step through a movement delta array
 *        and sends the resulting mouse report.
 *
 * The X axis is driven by @p deltas[phase] and the Y axis by
 * @p deltas[(phase + numdeltas/phasefraction) & (numdeltas-1)], creating a
 * phase-offset Lissajous-style path.  @p numdeltas **must** be a power of 2.
 *
 * @param deltas       Circular array of signed per-tick movement deltas.
 * @param numdeltas    Length of @p deltas (must be a power of 2).
 * @param phasefraction Divisor used to compute the Y-axis phase offset.
 * @param scalex       Multiplier applied to the X delta.
 * @param scaley       Multiplier applied to the Y delta.
 * @param randomdelay  When true, adds a phase-dependent jitter to the delay.
 * @param basedelay    Base delay in milliseconds between ticks.
 * @return Milliseconds until the next tick.
 */
uint32_t jiggler_pattern(int8_t deltas[], int8_t numdeltas, int8_t phasefraction, int8_t scalex, int8_t scaley,
                         bool randomdelay, int16_t basedelay) {
    static uint8_t phase = 0;
    uint32_t       delay;
    msJigReport.x = scalex * deltas[phase];
    msJigReport.y = scaley * deltas[(phase + (numdeltas / phasefraction)) & (numdeltas - 1)];
    host_mouse_send(&msJigReport);
    phase = (phase + 1) & (numdeltas - 1);
    if (randomdelay) {
        delay = basedelay + deltas[phase] * basedelay / 4 + phase * basedelay / 10;
    } else {
        delay = basedelay;
    }
    if (delay >= 10000) {
        pd_dprintf("msjg; next: %ds\n", (uint16_t)(delay / 1000));
    } else {
        pd_dprintf("msjg; next: %dms\n", (uint16_t)(delay));
    }
    return delay;
}

/**
 * @brief Cancels any active intro/outro sequence immediately.
 *
 * Cancels both the intro pattern token and the intro timeout timer token
 * if they are currently running.
 */
void jiggler_intro_end(void) {
    if (msJigIntroToken != INVALID_DEFERRED_TOKEN) {
        pd_dprintf("jiggle end of intro/outro\n");
        cancel_deferred_exec(msJigIntroToken);
        msJigIntroToken = INVALID_DEFERRED_TOKEN;
    }
    if (msJigIntroTimerToken != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec(msJigIntroTimerToken);
        msJigIntroTimerToken = INVALID_DEFERRED_TOKEN;
    }
}

/**
 * @name Movement delta arrays
 * @note Array lengths must be a power of 2 for the phase-wrap logic in
 *       jiggler_pattern() to work correctly.
 * @{
 */
/** @brief 32-entry delta table that traces a circle (or figure-eight). */
int8_t circledeltas[32] = {0, -1, -2, -2, -3, -3, -4, -4, -4, -4, -3, -3, -2, -2, -1, 0,
                           0, 1,  2,  2,  3,  3,  4,  4,  4,  4,  3,  3,  2,  2,  1,  0};
/** @brief 16-entry delta table for a small, irregular "subtle" motion. */
int8_t subtledeltas[16] = {1, -1, 1, 1, -2, 2, -2, -2, 2, -2, 2, 2, -1, 1, -1, -1};
/** @brief 16-entry delta table that traces a square path. */
int8_t squaredeltas[16] = {1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0};
/** @} */

/**
 * @name Deferred-exec pattern callbacks
 * Each callback is registered with defer_exec() and returns the delay until
 * its next invocation. The @p trigger_time and @p cb_arg parameters are
 * supplied by the deferred-exec subsystem and are unused by these callbacks.
 * @{
 */

/**
 * @brief Clockwise circle pattern, normal size (64 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_circle(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, -2, 2, 0, 64);
}

/**
 * @brief Clockwise circle pattern, small size (24 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_circle_small(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, -1, 1, 0, 24);
}

/**
 * @brief Counter-clockwise circle pattern, normal size (64 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_circle_ccw(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, 2, 2, 0, 64);
}

/**
 * @brief Counter-clockwise circle pattern, small size (24 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_circle_ccw_small(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, 1, 1, 0, 24);
}

/**
 * @brief Square path pattern (64 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_square(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(squaredeltas, 16, 4, 2, 2, 0, 64);
}

/**
 * @brief Large figure-eight pattern (64 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_figure(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, 4, 4, 0, 64);
}

/**
 * @brief Subtle random-delay jitter pattern (~16 s base tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_subtle(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(subtledeltas, 16, 4, 1, 1, 1, 16384);
}

/**
 * @brief Horizontal line pattern (X axis only, 24 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_xline(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, 1, 0, 0, 24);
}

/**
 * @brief Vertical line pattern (Y axis only, 24 ms/tick).
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return Delay in milliseconds until the next tick.
 */
uint32_t jiggler_yline(uint32_t trigger_time, void *cb_arg) {
    return jiggler_pattern(circledeltas, 32, 4, 0, 1, 0, 24);
}

/** @} */

/**
 * @brief Lookup table mapping @c PD_JIGGLER_PATTERN_* IDs to their
 *        deferred-exec callbacks.  Index 0 is reserved/invalid; indices 1–9
 *        correspond to the pattern constants defined in mouse_jiggler.h.
 */
static deferred_exec_callback const pattern_callbacks[] = {
    NULL,                     /* 0 – invalid/reserved               */
    jiggler_subtle,           /* 1 – PD_JIGGLER_PATTERN_SUBTLE       */
    jiggler_xline,            /* 2 – PD_JIGGLER_PATTERN_XLINE        */
    jiggler_yline,            /* 3 – PD_JIGGLER_PATTERN_YLINE        */
    jiggler_circle,           /* 4 – PD_JIGGLER_PATTERN_CIRCLE       */
    jiggler_circle_small,     /* 5 – PD_JIGGLER_PATTERN_CIRCLESMALL  */
    jiggler_circle_ccw,       /* 6 – PD_JIGGLER_PATTERN_CIRCLECCW    */
    jiggler_circle_ccw_small, /* 7 – PD_JIGGLER_PATTERN_CIRCLECCWSMALL */
    jiggler_figure,           /* 8 – PD_JIGGLER_PATTERN_FIGURE       */
    jiggler_square,           /* 9 – PD_JIGGLER_PATTERN_SQUARE       */
};

/** @brief Total entries in pattern_callbacks (including the NULL sentinel at index 0). */
#define PD_JIGGLER_NUM_PATTERNS (sizeof(pattern_callbacks) / sizeof(pattern_callbacks[0]))

/**
 * @brief Returns the deferred-exec callback for pattern @p id,
 *        or NULL if @p id is out of range or refers to the reserved slot.
 */
static inline deferred_exec_callback pattern_cb(uint8_t id) {
    return (id < PD_JIGGLER_NUM_PATTERNS) ? pattern_callbacks[id] : NULL;
}

/**
 * @brief Deferred-exec callback that forcibly ends the intro/outro sequence
 *        after @c PD_JIGGLER_INTRO_TIMEOUT milliseconds.
 *
 * Returns 0 so the deferred-exec subsystem does not reschedule it.
 *
 * @param trigger_time Scheduled trigger time (unused).
 * @param cb_arg       Callback argument (unused).
 * @return 0 (do not reschedule).
 */
uint32_t jiggler_introtimer(uint32_t trigger_time, void *cb_arg) {
    jiggler_intro_end();
    return 0;
}

/**
 * @brief Stops the primary jiggler loop and optionally plays the outro pattern.
 *
 * Cancels @c msJigMainToken, sends a zeroed mouse report to stop movement,
 * and (when intro is enabled via jiggler_set_intro_enabled()) schedules the
 * runtime-configured outro pattern (see jiggler_set_pattern_ending()).  When
 * @c jiggler_intro_timeout is non-zero, a timer is also scheduled to cancel
 * the outro after that many milliseconds.
 */
void jiggler_end(void) {
    pd_dprintf("jiggler_end\n");
    cancel_deferred_exec(msJigMainToken);
    msJigReport = (report_mouse_t){}; // Clear the mouse.
    host_mouse_send(&msJigReport);
    if (jiggler_intro_enabled) {
        deferred_exec_callback outro_cb = pattern_cb(jiggler_active_ending);
        if (outro_cb != NULL) {
            pd_dprintf("jiggler_end outro: pattern %d\n", jiggler_active_ending);
            msJigIntroToken = defer_exec(1, outro_cb, NULL);
        }
        if (jiggler_intro_timeout > 0) {
            msJigIntroTimerToken = defer_exec(jiggler_intro_timeout, jiggler_introtimer, NULL);
        }
    }
    msJigMainToken = INVALID_DEFERRED_TOKEN;
}

/**
 * @brief Starts the primary jiggler loop and optionally plays the intro pattern.
 *
 * Schedules the runtime-configured main pattern (see jiggler_set_pattern()) as
 * a deferred callback.  When intro is enabled (see jiggler_set_intro_enabled()),
 * also schedules the runtime-configured intro pattern
 * (see jiggler_set_pattern_intro()).  When @c jiggler_intro_timeout is non-zero
 * a timer is scheduled to cancel the intro after that many milliseconds.
 */
void jiggler_start(void) {
    deferred_exec_callback cb = pattern_cb(jiggler_active_pattern);
    if (cb != NULL) {
        pd_dprintf("jiggler_start: pattern %d\n", jiggler_active_pattern);
        msJigMainToken = defer_exec(1, cb, NULL);
    }
    if (jiggler_intro_enabled) {
        deferred_exec_callback intro_cb = pattern_cb(jiggler_active_intro);
        if (intro_cb != NULL) {
            pd_dprintf("intro: pattern %d\n", jiggler_active_intro);
            msJigIntroToken = defer_exec(1, intro_cb, NULL);
        }
        if (jiggler_intro_timeout > 0) {
            pd_dprintf("intro timer: %" PRIu32 "ms\n", jiggler_intro_timeout);
            msJigIntroTimerToken = defer_exec(jiggler_intro_timeout, jiggler_introtimer, NULL);
        }
    }
}

/**
 * @brief Returns whether intro/outro animations are currently enabled.
 * @return @c true if intro/outro will play, @c false if disabled.
 */
bool jiggler_get_intro_enabled(void) {
    return jiggler_intro_enabled;
}

/**
 * @brief Enables or disables intro/outro animations at runtime.
 * @param enabled @c true to play intro/outro sequences, @c false to skip them.
 */
void jiggler_set_intro_enabled(bool enabled) {
    jiggler_intro_enabled = enabled;
}

/**
 * @brief Returns the current intro/outro timeout in milliseconds.
 * @return Timeout in ms, or 0 if the timeout is disabled.
 */
uint32_t jiggler_get_intro_timeout(void) {
    return jiggler_intro_timeout;
}

/**
 * @brief Sets the intro/outro animation timeout at runtime.
 * @param timeout_ms Timeout in milliseconds; pass 0 to disable.
 */
void jiggler_set_intro_timeout(uint32_t timeout_ms) {
    jiggler_intro_timeout = timeout_ms;
}

/**
 * @brief Returns the currently active primary pattern ID.
 * @return One of the @c PD_JIGGLER_PATTERN_* constants.
 */
uint8_t jiggler_get_pattern(void) {
    return jiggler_active_pattern;
}

/**
 * @brief Returns the currently active intro animation pattern ID.
 * @return One of the @c PD_JIGGLER_PATTERN_* constants.
 */
uint8_t jiggler_get_pattern_intro(void) {
    return jiggler_active_intro;
}

/**
 * @brief Returns the currently active outro animation pattern ID.
 * @return One of the @c PD_JIGGLER_PATTERN_* constants.
 */
uint8_t jiggler_get_pattern_ending(void) {
    return jiggler_active_ending;
}

/**
 * @brief Sets the primary movement pattern at runtime.
 *
 * The change takes effect the next time jiggler_start() is called.
 * Invalid pattern IDs (out of range or 0) are silently ignored.
 *
 * @param pattern One of the @c PD_JIGGLER_PATTERN_* constants.
 */
void jiggler_set_pattern(uint8_t pattern) {
    if (pattern_cb(pattern) != NULL) {
        jiggler_active_pattern = pattern;
        jiggle_delay(jiggler_backoff);
    }
}

/**
 * @brief Sets the intro animation pattern at runtime.
 *
 * The change takes effect the next time jiggler_start() is called.
 * Invalid pattern IDs are silently ignored.
 *
 * @param pattern One of the @c PD_JIGGLER_PATTERN_* constants.
 */
void jiggler_set_pattern_intro(uint8_t pattern) {
    if (pattern_cb(pattern) != NULL) {
        jiggler_active_intro = pattern;
        jiggle_delay(jiggler_backoff);
    }
}

/**
 * @brief Sets the outro animation pattern at runtime.
 *
 * The change takes effect the next time jiggler_end() is called.
 * Invalid pattern IDs are silently ignored.
 *
 * @param pattern One of the @c PD_JIGGLER_PATTERN_* constants.
 */
void jiggler_set_pattern_ending(uint8_t pattern) {
    if (pattern_cb(pattern) != NULL) {
        jiggler_active_ending = pattern;
        jiggle_delay(jiggler_backoff);
    }
}

/**
 * @brief Toggles the jiggler on or off.
 *
 * Cancels any active intro/outro first, then calls jiggler_end() if the
 * jiggler is currently running or jiggler_start() if it is stopped.
 */
void jiggler_toggle(void) {
    pd_dprintf("jiggler_toggle\n");
    jiggler_intro_end();
    if (jiggler_get_state()) {
        jiggler_end();
    } else {
        jiggler_start();
    }
}

/**
 * @brief Starts the jiggler if it is not already running.
 */
void jiggler_enable(void) {
    pd_dprintf("jiggler_enable\n");
    jiggler_intro_end();
    if (!jiggler_get_state()) {
        jiggler_start();
    }
}

/**
 * @brief Stops the jiggler if it is currently running.
 */
void jiggler_disable(void) {
    pd_dprintf("jiggler_disable\n");
    jiggler_intro_end();
    if (jiggler_get_state()) {
        jiggler_end();
    }
}

/**
 * @brief Returns whether autostop is currently enabled.
 * @return @c true if the jiggler stops automatically on any keypress.
 */
bool jiggler_get_autostop(void) {
    return jiggler_autostop;
}

/**
 * @brief Enables or disables autostop at runtime.
 * @param enabled @c true to stop the jiggler on any keypress, @c false to keep it running.
 */
void jiggler_set_autostop(bool enabled) {
    jiggler_autostop = enabled;
}

/**
 * @brief Returns the current backoff delay in seconds.
 * @return Backoff duration in seconds applied after any real user input.
 */
uint32_t jiggler_get_backoff(void) {
    return jiggler_backoff;
}

/**
 * @brief Sets the backoff delay at runtime and immediately applies it.
 *
 * Updates the backoff value and calls jiggle_delay() with the new value so
 * the change takes effect for the current jiggler cycle as well.
 *
 * @param backoff_sec Backoff duration in seconds; 0 disables the backoff.
 */
void jiggler_set_backoff(uint32_t backoff_sec) {
    jiggler_backoff = backoff_sec;
    jiggle_delay(backoff_sec);
}

/**
 * @brief Extends the next jiggler tick by @p delay_sec seconds.
 *
 * If the jiggler is active this pushes the next movement forward, preventing
 * simulated input from interfering with real user activity.
 *
 * @param delay_sec Additional delay in seconds to apply.
 */
void jiggle_delay(uint32_t delay_sec) {
    if (jiggler_get_state()) {
        // pd_dprintf("delay the jiggles\n");
        extend_deferred_exec(msJigMainToken, delay_sec * 1000);
    }
}

/**
 * @brief Key-event hook for the mouse jiggler module.
 *
 * Handles the @c COMMUNITY_MODULE_MOUSE_JIGGLER_TOGGLE keycode and, when
 * autostop is enabled (see jiggler_set_autostop()), stops the jiggler on any
 * keypress.  Also calls jiggle_delay() on every key event so that real input
 * does not collide with simulated mouse movement.
 *
 * @param keycode The keycode of the key event.
 * @param record  Pointer to the key record containing event details.
 * @return true to continue normal QMK key processing, false to consume the event.
 */
bool process_record_mouse_jiggler(uint16_t keycode, keyrecord_t *record) {
    // Delay the next jiggler tick to avoid simulated movement colliding with real input.
    jiggle_delay(jiggler_backoff);
    if (record->event.pressed) {
        if (keycode == COMMUNITY_MODULE_MOUSE_JIGGLER_TOGGLE) {
            jiggler_toggle();
            return false;
        }
        if (jiggler_autostop && jiggler_get_state()) {
            jiggler_intro_end();
            jiggler_end();
        }
    }
    return true;
}

/**
 * @brief Pointing-device hook for the mouse jiggler module.
 *
 * Delays the next simulated movement whenever real pointing-device activity
 * is detected (non-zero X, Y, H, or V), preventing the two from fighting.
 *
 * @param mouse_report The current pointing-device report from the hardware.
 * @return The unmodified @p mouse_report (passed through unchanged).
 */
report_mouse_t pointing_device_task_mouse_jiggler(report_mouse_t mouse_report) {
    if (mouse_report.x || mouse_report.y || mouse_report.h || mouse_report.v) {
        jiggle_delay(jiggler_backoff);
    }
    return mouse_report;
}
