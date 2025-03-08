RTC_DRIVER ?= vendor
VALID_RTC_DRIVER_TYPES := vendor custom ds3231 ds1307 pcf8523

ifeq ($(filter $(RTC_DRIVER),$(VALID_RTC_DRIVER_TYPES)),)
    $(call CATASTROPHIC_ERROR,Invalid RTC_DRIVER,RTC_DRIVER="$(RTC_DRIVER)" is not a valid RTC type)
else
    OPT_DEFS += -D$(strip $(shell echo $(RTC_DRIVER) | tr '[:lower:]' '[:upper:]'))_RTC_DRIVER_ENABLE
    SRC += modules/drashna/rtc/drivers/$(strip $(RTC_DRIVER)).c
    ifeq ($(strip $(RTC_FORCE_INIT)), yes)
        OPT_DEFS += -DRTC_FORCE_INIT
    endif
    ifeq ($(strip $(RTC_DRIVER)), vendor)
        OPT_DEFS += -DHAL_USE_RTC=TRUE
    else
        I2C_DRIVER_REQUIRED = yes
    endif
endif
