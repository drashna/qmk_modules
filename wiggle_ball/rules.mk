WIGGLE_BALL_INDEX := $(wordlist 1,$(words $(COMMUNITY_MODULES)),$(COMMUNITY_MODULES))
DRAG_SCROLL_INDEX := $(filter drag_scroll,$(wordlist 1,$(words $(WIGGLE_BALL_INDEX)),$(COMMUNITY_MODULES)))

# Super hack to check if drag scroll is enabled before wiggle ball, so that we can check if drag scroll is active
# in the wiggle ball code without requiring users to mess with things.
ifneq ($(DRAG_SCROLL_INDEX),)
    OPT_DEFS += -DWIGGLE_BALL_DRAG_SCROLL_LOADED_FIRST
endif
