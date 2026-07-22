# We need to disable LTO for this file, because the linker will optimize away the wrapper functions otherwise.
# This is a known issue with GCC and LTO, and the workaround is to disable LTO for the specific object file that
# contains the wrapper functions.
LDFLAGS += -Wl,--wrap=dynamic_keymap_set_keycode,--wrap=dynamic_keymap_set_encoder,--wrap=dynamic_keymap_set_buffer
%/quantum/dynamic_keymap.o: EXTRAFLAGS += -fno-lto
