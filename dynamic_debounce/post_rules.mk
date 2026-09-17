# This module implements the `debounce()`/`debounce_init()` API itself, so
# prevent common_features.mk from also pulling in a stock quantum/debounce/*.c
# implementation (which would collide at link time).

# Map a keyboard/keymap-specified DEBOUNCE_TYPE to the matching
# DYNAMIC_DEBOUNCE_DEFAULT_ALGO enumerator, so existing DEBOUNCE_TYPE settings
# still pick the same algorithm as their initial default.
DYNAMIC_DEBOUNCE_ALGO_sym_defer_g := DYNAMIC_DEBOUNCE_SYM_DEFER_G
DYNAMIC_DEBOUNCE_ALGO_sym_eager_pr := DYNAMIC_DEBOUNCE_SYM_EAGER_PR
DYNAMIC_DEBOUNCE_ALGO_sym_defer_pr := DYNAMIC_DEBOUNCE_SYM_DEFER_PR
DYNAMIC_DEBOUNCE_ALGO_sym_eager_pk := DYNAMIC_DEBOUNCE_SYM_EAGER_PK
DYNAMIC_DEBOUNCE_ALGO_sym_defer_pk := DYNAMIC_DEBOUNCE_SYM_DEFER_PK
DYNAMIC_DEBOUNCE_ALGO_asym_eager_defer_pk := DYNAMIC_DEBOUNCE_ASYM_EAGER_DEFER_PK
DYNAMIC_DEBOUNCE_ALGO_none := DYNAMIC_DEBOUNCE_NONE

ifneq ($(strip $(DEBOUNCE_TYPE)),)
    ifneq ($(strip $(DEBOUNCE_TYPE)), custom)
        DYNAMIC_DEBOUNCE_MAPPED_ALGO := $(DYNAMIC_DEBOUNCE_ALGO_$(strip $(DEBOUNCE_TYPE)))
        ifeq ($(strip $(DYNAMIC_DEBOUNCE_MAPPED_ALGO)),)
            $(call CATASTROPHIC_ERROR,Invalid DEBOUNCE_TYPE,DEBOUNCE_TYPE="$(DEBOUNCE_TYPE)" is not supported by drashna/dynamic_debounce)
        else
            OPT_DEFS += -DDYNAMIC_DEBOUNCE_DEFAULT_ALGO=$(DYNAMIC_DEBOUNCE_MAPPED_ALGO)
        endif
    endif
endif

override DEBOUNCE_TYPE := custom
