RTC_DRIVER ?= vendor
VALID_RTC_DRIVER_TYPES := vendor custom ds3231 ds1307 pcf8523

ifeq ($(filter $(RTC_DRIVER),$(VALID_RTC_DRIVER_TYPES)),)
    $(call CATASTROPHIC_ERROR,Invalid RTC_DRIVER,RTC_DRIVER="$(RTC_DRIVER)" is not a valid RTC type)
else
    OPT_DEFS += -D$(strip $(shell echo $(RTC_DRIVER) | tr '[:lower:]' '[:upper:]'))_RTC_DRIVER_ENABLE
    ifeq ($(strip $(RTC_DRIVER)), vendor)
        OPT_DEFS += -DHAL_USE_RTC=TRUE
        SRC += drivers/$(strip $(RTC_DRIVER))_$(PLATFORM_KEY).c
        ifeq ($(strip $(MCU)), RP2040)
            RTC_FORCE_INIT = yes
            $(call WARNING_MESSAGE,RP2040 RTC does not retain time across resets. You must set the time or sync it with an external source.)
        endif
    else
        SRC += drivers/$(strip $(RTC_DRIVER)).c
        I2C_DRIVER_REQUIRED = yes
    endif
    ifeq ($(strip $(RTC_FORCE_INIT)), yes)
        OPT_DEFS += -DRTC_FORCE_INIT
    endif
endif
