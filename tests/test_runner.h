/*
 * test_runner.h — minimal unit test macros for host-native tests
 *
 * Usage:
 *   - Call RUN(fn) in main() for each void test function.
 *   - Use ASSERT(expr) inside test functions.
 *   - End main() with REPORT("suite name").
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdio.h>

static int _passes   = 0;
static int _failures = 0;

#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "      assertion failed: %s (%s:%d)\n", \
                    #expr, __FILE__, __LINE__); \
            _failures++; \
        } else { \
            _passes++; \
        } \
    } while (0)

/* Run a test function and print a single pass/FAIL line. */
#define RUN(fn) \
    do { \
        int _before = _failures; \
        fn(); \
        printf("  %-50s %s\n", #fn, (_failures == _before) ? "pass" : "FAIL"); \
    } while (0)

/* Print totals and return an appropriate exit code from main(). */
#define REPORT(suite) \
    do { \
        printf("\n%s: %d passed, %d failed\n", (suite), _passes, _failures); \
        return (_failures > 0) ? 1 : 0; \
    } while (0)

#endif /* TEST_RUNNER_H */
