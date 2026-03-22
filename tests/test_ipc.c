/*
 * test_ipc.c — host-native unit tests for the IPC message queue.
 *
 * ipc.c uses tinylibc_printf for diagnostics; that symbol is stubbed in
 * stubs.c so error-path output is suppressed during tests.
 */

#include <stdio.h>
#include <string.h>

#include "test_runner.h"
#include "../drivers/ipc.h"

/* --- tests ---------------------------------------------------------------- */

static void test_empty_after_init(void)
{
    ipc_init();
    char buf[32];
    uint8_t len;
    ASSERT(ipc_read(buf, sizeof(buf), &len) != ERR_OK);
}

static void test_write_read_roundtrip(void)
{
    ipc_init();
    ASSERT(ipc_write("hello", 6) == ERR_OK); /* len includes the null terminator */
    char buf[32];
    uint8_t len;
    ASSERT(ipc_read(buf, sizeof(buf), &len) == ERR_OK);
    ASSERT(strcmp(buf, "hello") == 0);
    ASSERT(len == 5);
}

static void test_fifo_ordering(void)
{
    ipc_init();
    ipc_write("first",  6);
    ipc_write("second", 7);
    ipc_write("third",  6);
    char buf[32];
    uint8_t len;
    ipc_read(buf, sizeof(buf), &len); ASSERT(strcmp(buf, "first")  == 0);
    ipc_read(buf, sizeof(buf), &len); ASSERT(strcmp(buf, "second") == 0);
    ipc_read(buf, sizeof(buf), &len); ASSERT(strcmp(buf, "third")  == 0);
    ASSERT(ipc_read(buf, sizeof(buf), &len) != ERR_OK);
}

static void test_write_full_fails(void)
{
    ipc_init();
    for (int i = 0; i < IPC_BUFFER_SIZE; i++)
        ASSERT(ipc_write("x", 2) == ERR_OK);
    ASSERT(ipc_write("x", 2) != ERR_OK); /* one beyond capacity */
}

static void test_read_empty_fails(void)
{
    ipc_init();
    char buf[32];
    uint8_t len;
    ASSERT(ipc_read(buf, sizeof(buf), &len) != ERR_OK);
}

static void test_write_len_at_limit_fails(void)
{
    /* len must be < IPC_DATA_SIZE to leave room for the null terminator. */
    ipc_init();
    ASSERT(ipc_write("x", IPC_DATA_SIZE) != ERR_OK);
}

static void test_write_not_null_terminated_fails(void)
{
    /* len=3 but "abc" has no null within the first 3 bytes. */
    ipc_init();
    ASSERT(ipc_write("abc", 3) != ERR_OK);
}

static void test_read_output_buf_too_small_fails(void)
{
    ipc_init();
    ipc_write("hello", 6); /* strlen == 5 */
    char buf[4];
    uint8_t len;
    /* max_len=4, but stored string length is 5 → 5 >= 4 → fail */
    ASSERT(ipc_read(buf, 4, &len) != ERR_OK);
}

static void test_failed_read_does_not_consume_message(void)
{
    ipc_init();
    ipc_write("hello", 6);
    char buf[4];
    uint8_t len;
    ASSERT(ipc_read(buf, 4, &len) != ERR_OK); /* fails */

    char buf2[32];
    ASSERT(ipc_read(buf2, sizeof(buf2), &len) == ERR_OK); /* message still there */
    ASSERT(strcmp(buf2, "hello") == 0);
}

static void test_count_tracks_correctly(void)
{
    ipc_init();
    char buf[32];
    uint8_t len;

    ipc_write("a", 2);
    ipc_write("b", 2);
    ipc_read(buf, sizeof(buf), &len); /* consume "a" */
    ipc_write("c", 2);

    ipc_read(buf, sizeof(buf), &len); ASSERT(strcmp(buf, "b") == 0);
    ipc_read(buf, sizeof(buf), &len); ASSERT(strcmp(buf, "c") == 0);
    ASSERT(ipc_read(buf, sizeof(buf), &len) != ERR_OK);
}

static void test_wraparound(void)
{
    /* Advance head and tail to the midpoint, then fill to force index wrap. */
    ipc_init();
    char buf[32];
    uint8_t len;

    for (int i = 0; i < IPC_BUFFER_SIZE / 2; i++)
        ipc_write("init", 5);
    for (int i = 0; i < IPC_BUFFER_SIZE / 2; i++)
        ipc_read(buf, sizeof(buf), &len);

    /* Head and tail are at midpoint; fill to full capacity with a new string. */
    for (int i = 0; i < IPC_BUFFER_SIZE; i++)
        ASSERT(ipc_write("wrap", 5) == ERR_OK);

    ASSERT(ipc_write("x", 2) != ERR_OK); /* should be full */

    for (int i = 0; i < IPC_BUFFER_SIZE; i++) {
        ASSERT(ipc_read(buf, sizeof(buf), &len) == ERR_OK);
        ASSERT(strcmp(buf, "wrap") == 0);
    }
    ASSERT(ipc_read(buf, sizeof(buf), &len) != ERR_OK);
}

/* --- entry point ---------------------------------------------------------- */

int main(void)
{
    printf("ipc\n");
    RUN(test_empty_after_init);
    RUN(test_write_read_roundtrip);
    RUN(test_fifo_ordering);
    RUN(test_write_full_fails);
    RUN(test_read_empty_fails);
    RUN(test_write_len_at_limit_fails);
    RUN(test_write_not_null_terminated_fails);
    RUN(test_read_output_buf_too_small_fails);
    RUN(test_failed_read_does_not_consume_message);
    RUN(test_count_tracks_correctly);
    RUN(test_wraparound);
    REPORT("ipc");
}
