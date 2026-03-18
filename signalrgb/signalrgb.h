#pragma once

enum signalrgb_commands {
    GET_QMK_VERSION            = 0x21,
    GET_PROTOCOL_VERSION       = 0x22,
    GET_UNIQUE_IDENTIFIER      = 0x23,
    STREAM_RGB_DATA            = 0x24,
    SET_SIGNALRGB_MODE_ENABLE  = 0x25,
    SET_SIGNALRGB_MODE_DISABLE = 0x26,
    GET_TOTAL_LEDS             = 0x27,
    GET_FIRMWARE_TYPE          = 0x28,
};

enum signalrgb_responses {
    PROTOCOL_VERSION_BYTE_1         = 1,
    PROTOCOL_VERSION_BYTE_2         = 0,
    PROTOCOL_VERSION_BYTE_3         = 6,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_1 = 0,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_2 = 0,
    DEVICE_UNIQUE_IDENTIFIER_BYTE_3 = 0,
    FIRMWARE_TYPE_BYTE              = 2,
    DEVICE_ERROR_LED_BOUNDS         = 253,
    DEVICE_ERROR_LED_COUNT          = 254
};

void get_qmk_version(void);
void get_signalrgb_protocol_version(void);
void get_unique_identifier(void);
void led_streaming(uint8_t *data);
void signalrgb_mode_enable(void);
void signalrgb_mode_disable(void);
void get_total_leds(void);
