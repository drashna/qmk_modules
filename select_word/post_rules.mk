ifneq ($(strip $(OS_DETECTION_ENABLE)), yes)
    $(shell $(QMK_BIN) hello -n "OS Detection isn't required for the Select Word feature to work, but does improve it.")
endif
