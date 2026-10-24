/*
 * Unit tests for internal/account.
 *
 * Points HOME (or USERPROFILE) at a temp dir so we never touch the real
 * user's ~/.meowlaunch.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/account.c"
#include "internal/account/uuid.c"
#include "internal/account/usersfile.c"

static int failures = 0;

#define CHECK(cond, name)                                              \
    do {                                                               \
        if (cond) { printf("[PASS] %s\n", name); }                     \
        else { printf("[FAIL] %s\n", name); failures++; }              \
    } while (0)

int main(void)
{
    const char *tmp = getenv("MEOW_TEST_HOME");
    if (tmp == NULL) tmp = "/tmp/meow_acct_test";
#ifdef _WIN32
    {
        char buf[256];
        snprintf(buf, sizeof buf, "USERPROFILE=%s", tmp);
        putenv(buf);
    }
#else
    setenv("HOME", tmp, 1);
    char mk[300];
    snprintf(mk, sizeof mk, "rm -rf %s && mkdir -p %s", tmp, tmp);
    system(mk);
#endif

    CHECK(Account_Local_Add("alice") == 0, "add alice succeeds");
    CHECK(Account_Local_Add("bob") == 0, "add bob succeeds");
    CHECK(Account_Local_Add("alice") != 0, "duplicate add rejected");
    CHECK(Account_Local_Del("alice") == 0, "del alice succeeds");
    CHECK(Account_Local_Del("nobody") != 0, "del unknown fails");

    char uuid[37];
    uuid_generate(uuid, sizeof uuid);
    CHECK(strlen(uuid) == 36 &&
          uuid[8] == '-' && uuid[13] == '-' &&
          uuid[18] == '-' && uuid[23] == '-',
          "uuid is 8-4-4-4-12");

    return failures ? 1 : 0;
}
