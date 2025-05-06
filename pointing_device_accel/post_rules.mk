ifeq ($(strip $(VIA_ENABLE)), yes)
    ifeq ($(strip $(POINTING_DEVICE_ACCEL_VIA_ENABLE)), yes)
        OPT_DEFS += -DPOINTING_DEVICE_ACCEL_VIA_ENABLE
        SRC += pointing_device_accel_via.c
    endif
endif
