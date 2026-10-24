/*
 * Unit tests for internal/account/usersfile.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/usersfile.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

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
        char p[1024];
        CHECK(usersfile_path(p, sizeof p) == 0 && strstr(p, "users.json") != NULL,
              "usersfile_path returns users.json path");
    }

    {
        char *e = usersfile_load("/nonexistent/___no_such___.json");
        CHECK(e != NULL, "usersfile_load missing returns empty");
        free(e);
    }

    {
        char *buf = strdup("{\"users\":[]}");
        CHECK(usersfile_insert(&buf, "u1", "uuid-1") == 0, "insert into empty");
        CHECK(usersfile_insert(&buf, "u2", "uuid-2") == 0, "insert second");
        CHECK(strstr(buf, "u1") && strstr(buf, "u2"), "both present");
        CHECK(usersfile_insert(&buf, "u1", "x") == -1, "insert duplicate rejected");
        CHECK(usersfile_remove(&buf, "u2") == 0, "remove middle (comma branch)");
        CHECK(strstr(buf, "u2") == NULL && strstr(buf, "u1") != NULL, "removed gone");
        CHECK(usersfile_remove(&buf, "nobody") == -1, "remove missing fails");
        free(buf);
    }

    {
        CHECK(usersfile_save("build_test/usersfile_test.json", "{\"users\":[]}") == 0,
              "usersfile_save writes");
        char *r = usersfile_load("build_test/usersfile_test.json");
        CHECK(r != NULL && strstr(r, "users") != NULL, "usersfile_save load back");
        free(r);
    }

    return failures ? 1 : 0;
}
