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
    /* Wipe any leftover users.json next to the test binary (build_test/). */
    remove("build_test/.meowlaunch/data/users.json");
    remove("build_test/.meowlaunch/data");
    remove("build_test/.meowlaunch");

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

    CHECK(Account_Local_Show() == 0, "account local show runs");

    return failures ? 1 : 0;
}
