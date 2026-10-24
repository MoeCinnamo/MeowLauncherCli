/*
 * Unit tests for internal/account/account.c (public API).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/account.c"
#include "internal/account/uuid.c"
#include "internal/account/usersfile.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

const char *I18n(const char *key) { return key; }

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    const char *tmp = getenv("MEOW_TEST_HOME");
    if (tmp == NULL) tmp = "/tmp/meow_acct_test";
#ifdef _WIN32
    { char b[256]; snprintf(b, sizeof b, "USERPROFILE=%s", tmp); putenv(b); }
#else
    setenv("HOME", tmp, 1);
#endif
    {
        char p[1200];
        snprintf(p, sizeof p,
#ifdef _WIN32
                 "%s\\.meowlaunch\\data\\users.json",
#else
                 "%s/.meowlaunch/data/users.json",
#endif
                 tmp);
        remove(p);
    }

    CHECK(Account_Local_Add("alice") == 0, "add alice succeeds");
    CHECK(Account_Local_Add("bob") == 0, "add bob succeeds");
    CHECK(Account_Local_Add("alice") != 0, "duplicate add rejected");
    CHECK(Account_Local_Del("alice") == 0, "del alice succeeds");
    CHECK(Account_Local_Del("nobody") != 0, "del unknown fails");

    /* Empty / NULL name paths. */
    CHECK(Account_Local_Add("") != 0, "add empty name fails");
    CHECK(Account_Local_Add(NULL) != 0, "add NULL name fails");
    CHECK(Account_Local_Del("") != 0, "del empty name fails");
    CHECK(Account_Local_Del(NULL) != 0, "del NULL name fails");

    /* Home unset: usersfile_path fails. */
#ifdef _WIN32
    putenv("USERPROFILE=");
#else
    unsetenv("HOME");
#endif
    CHECK(Account_Local_Add("x") != 0, "add with no home fails");
    CHECK(Account_Local_Del("x") != 0, "del with no home fails");

    return failures ? 1 : 0;
}
