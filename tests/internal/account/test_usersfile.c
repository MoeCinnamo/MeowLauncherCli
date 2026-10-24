/*
 * Unit tests for internal/account/usersfile.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/usersfile.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

static int malloc_fail_at = -1;
static int malloc_calls = 0;

/* Linker wraps: --wrap,malloc routes malloc -> __wrap_malloc. */
void *__real_malloc(size_t);
void __real_free(void *);
void *__wrap_malloc(size_t n)
{
    malloc_calls++;
    if (malloc_fail_at >= 0 && malloc_calls > malloc_fail_at) {
        return NULL;
    }
    return __real_malloc(n);
}
void __wrap_free(void *p) { __real_free(p); }

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

    /* home unset -> usersfile_path returns -1. */
#ifdef _WIN32
    putenv("USERPROFILE=");
#else
    unsetenv("HOME");
#endif
    {
        char p[1024];
        CHECK(usersfile_path(p, sizeof p) != 0, "usersfile_path home NULL -> -1");
    }
#ifdef _WIN32
    { char b[256]; snprintf(b, sizeof b, "USERPROFILE=%s", tmp); putenv(b); }
#else
    setenv("HOME", tmp, 1);
#endif

    /* insert with no ']' -> -1. */
    {
        char *bad = strdup("{\"users\":oops");
        CHECK(usersfile_insert(&bad, "x", "y") == -1, "insert no bracket -> -1");
        free(bad);
    }

    /* remove with no closing '}' -> -1. */
    {
        char *bad = strdup("[{\"name\":\"x\"");
        CHECK(usersfile_remove(&bad, "x") == -1, "remove no brace -> -1");
        free(bad);
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

    /* malloc failure during insert: inject failure so nb=malloc returns NULL. */
    {
        char *buf = strdup("{\"users\":[]}");
        malloc_calls = 0;
        malloc_fail_at = 0; /* first malloc in insert (nb) fails */
        CHECK(usersfile_insert(&buf, "x", "y") == -1, "insert malloc fail -> -1");
        free(buf);
        malloc_fail_at = -1;
    }

    return failures ? 1 : 0;
}
