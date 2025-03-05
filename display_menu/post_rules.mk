ifeq ($(strip $(OLED_ENABLE)), yes)
    SRC += oled_render_menu.c
endif

ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)), yes)
    SRC += qp_render_menu.c
endif
