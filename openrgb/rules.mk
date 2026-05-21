# Patch usb_descriptor.h to make RAW_EPSIZE overridable (required for OpenRGB's 64-byte protocol)
USB_DESCRIPTOR_H := $(PROTOCOL_PATH)/usb_descriptor.h
$(shell \
    if grep -q '^#define RAW_EPSIZE 32$$' "$(USB_DESCRIPTOR_H)"; then \
        sed -i 's/^#define RAW_EPSIZE 32$$/#ifndef RAW_EPSIZE\n#    define RAW_EPSIZE 32\n#endif/' "$(USB_DESCRIPTOR_H)"; \
    fi \
)
