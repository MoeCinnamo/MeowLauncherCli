/*
 * Tests for internal/paths/paths.c.
 */

#include <stdio.h>
#include <string.h>

#include "internal/paths/paths.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    char buf[1024];
    CHECK(ExeDir(buf, sizeof buf) == 0, "exedir resolves");
    CHECK(strlen(buf) > 0, "exedir non-empty");
    return failures ? 1 : 0;
}
