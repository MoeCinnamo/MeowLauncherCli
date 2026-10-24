/*
 * Unit tests for internal/account/uuid.c.
 */

#include <stdio.h>
#include <string.h>

#include "internal/account/uuid.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    char uuid[37];
    uuid_generate(uuid, sizeof uuid);
    CHECK(strlen(uuid) == 36 &&
          uuid[8] == '-' && uuid[13] == '-' &&
          uuid[18] == '-' && uuid[23] == '-',
          "uuid is 8-4-4-4-12");
    return failures ? 1 : 0;
}
