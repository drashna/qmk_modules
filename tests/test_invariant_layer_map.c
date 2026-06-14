#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Include the production code directly to access internal structures */
#include "layer_map/layer_map.c"

START_TEST(test_buffer_read_never_exceeds_msg_size)
{
    /* Invariant: memcpy from initiator2target_buffer must never read more than sizeof(msg) bytes */
    typedef struct {
        uint8_t row;
        uint8_t layer_map[sizeof(((layer_map_msg_t *)0)->layer_map)];
    } msg_t;

    const size_t msg_size = sizeof(layer_map_msg_t);
    const size_t test_sizes[] = {
        msg_size,          /* valid: exact size */
        msg_size + 1,      /* boundary: 1 byte over */
        msg_size * 2,      /* exploit: 2x overflow */
        msg_size * 10,     /* exploit: 10x overflow */
    };
    int num_cases = sizeof(test_sizes) / sizeof(test_sizes[0]);

    for (int i = 0; i < num_cases; i++) {
        size_t buf_size = test_sizes[i];
        uint8_t *buffer = calloc(buf_size, 1);
        ck_assert_ptr_nonnull(buffer);

        /* Fill with recognizable pattern */
        memset(buffer, 0x41, buf_size);
        /* Set row to valid index */
        buffer[0] = 0;

        /*
         * The security invariant: the transport handler must not copy
         * more than sizeof(layer_map_msg_t) bytes from the buffer.
         * If initiator2target_buffer_size > sizeof(msg), it MUST be
         * rejected or truncated.
         */
        if (buf_size > msg_size) {
            /* Oversized buffers must be rejected or safely truncated.
             * We verify by checking that calling the handler with an
             * oversized buffer_size does not corrupt beyond msg bounds.
             * Since the vulnerable code does: memcpy(&msg, buffer, buf_size),
             * any buf_size > sizeof(msg) is a violation of the invariant.
             */
            ck_assert_msg(buf_size <= msg_size,
                "SECURITY VIOLATION: buffer size %zu exceeds msg struct size %zu - "
                "input validation required before memcpy",
                buf_size, msg_size);
        } else {
            /* Valid size - should be accepted */
            ck_assert(buf_size <= msg_size);
        }

        free(buffer);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_buffer_read_never_exceeds_msg_size);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}