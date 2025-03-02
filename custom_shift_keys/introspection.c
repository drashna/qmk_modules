// Copyright 2025 drashna
// SPDX-License-Identifier: GPL-2.0-or-later
#ifdef COMMUNITY_MODULE_CUSTOM_SHIFT_KEYS_ENABLE

uint16_t custom_shift_keys_count_raw(void) {
    return ARRAY_SIZE(custom_shift_keys);
}
__attribute__((weak)) uint16_t custom_shift_keys_count(void) {
    return custom_shift_keys_count_raw();
}

custom_shift_key_t* custom_shift_keys_get_raw(uint16_t custom_shift_keys_idx) {
    if (custom_shift_keys_idx >= custom_shift_keys_count_raw()) {
        return NULL;
    }
    return &custom_shift_keys[custom_shift_keys_idx];
}

__attribute__((weak)) custom_shift_key_t* custom_shift_keys_get(uint16_t custom_shift_keys_idx) {
    return custom_shift_keys_get_raw(custom_shift_keys_idx);
}

#endif // COMMUNITY_MODULE_SOCD_CLEANER_ENABLE
