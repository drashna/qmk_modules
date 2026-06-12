#if !defined(OS_DETECTION_ENABLE) || (defined(OS_DETECTION_ENABLE) && !defined(OS_DETECTION_KEYBOARD_RESET))
#    include "timer.h"
#    include "quantum.h"

// to react on USB state changes
static volatile bool                    debouncing               = false;
static volatile fast_timer_t            last_time                = 0;
static volatile fast_timer_t            configured_since         = 0;
static volatile bool                    reset_pending            = false;
static volatile struct usb_device_state current_usb_device_state = {.configure_state = USB_DEVICE_STATE_NO_INIT};
static volatile struct usb_device_state maxprev_usb_device_state = {.configure_state = USB_DEVICE_STATE_NO_INIT};

#    ifndef KVM_DETECTION_DEBOUNCE
#        define KVM_DETECTION_DEBOUNCE 250
#    endif
#    if KVM_DETECTION_DEBOUNCE > 2000
#        undef KVM_DETECTION_DEBOUNCE
#        define KVM_DETECTION_DEBOUNCE 2000
#    endif

/**
 * @brief Handles deferred USB-state recovery and debounce timing during
 *        housekeeping for KVM-related USB instability.
 */
void housekeeping_task_kvm_helper(void) {
    // resetting the keyboard on the USB device state change callback results in instability, so delegate that to this
    // task
    if (reset_pending) {
        soft_reset_keyboard();
    }
    // reset the keyboard if it is stuck in the init state for longer than debounce duration, which can happen with some
    // KVMs
    if (current_usb_device_state.configure_state <= USB_DEVICE_STATE_INIT &&
        maxprev_usb_device_state.configure_state >= USB_DEVICE_STATE_CONFIGURED) {
        if (debouncing && timer_elapsed_fast(last_time) >= KVM_DETECTION_DEBOUNCE) {
            soft_reset_keyboard();
        }
        return;
    }

    if (current_usb_device_state.configure_state == USB_DEVICE_STATE_CONFIGURED) {
        // debouncing goes for both the detected OS as well as the USB state
        if (debouncing && timer_elapsed_fast(last_time) >= KVM_DETECTION_DEBOUNCE) {
            debouncing = false;
            last_time  = 0;
        }
    }

    housekeeping_task_kvm_helper_kb();
}

/**
 * @brief Tracks USB device state transitions and starts debounce/reset handling
 *        when host/KVM switching causes unstable enumeration.
 *
 * @param usb_device_state Current USB device state notification.
 */
void notify_usb_device_state_change_kvm_helper(struct usb_device_state usb_device_state) {
    // treat this like any other source of instability
    if (maxprev_usb_device_state.configure_state < current_usb_device_state.configure_state) {
        maxprev_usb_device_state.configure_state = current_usb_device_state.configure_state;
    }
    current_usb_device_state = usb_device_state;
    last_time                = timer_read_fast();
    debouncing               = true;

    if (configured_since == 0 && current_usb_device_state.configure_state == USB_DEVICE_STATE_CONFIGURED) {
        configured_since = timer_read_fast();
    } else if (current_usb_device_state.configure_state == USB_DEVICE_STATE_INIT) {
        // reset the keyboard only if it's been stable for at least debounce duration, to avoid issues with some KVMs
        if (configured_since > 0 && timer_elapsed_fast(configured_since) >= KVM_DETECTION_DEBOUNCE) {
            reset_pending = true;
        }
        configured_since = 0;
    }
}

/**
 * @brief Default user-level USB state-change hook.
 *
 * Forwards to the KVM helper implementation unless overridden.
 *
 * @param usb_device_state Current USB device state notification.
 */
__attribute__((weak)) void notify_usb_device_state_change_user(struct usb_device_state usb_device_state) {
    notify_usb_device_state_change_kvm_helper(usb_device_state);
}
#endif
