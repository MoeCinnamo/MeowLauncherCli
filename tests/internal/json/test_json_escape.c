/*
 * Unit tests for internal/json/json_escape.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json_escape.c"

static int malloc_fail_at = -1;
static int malloc_calls = 0;
#ifndef __APPLE__
void *__real_malloc(size_t);
void *__wrap_malloc(size_t n)
{
    malloc_calls++;
    if (malloc_fail_at >= 0 && malloc_calls > malloc_fail_at) return NULL;
    return __real_malloc(n);
}
#endif

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    {
        char *e = Json_Escape("1.21.11");
        CHECK(e != NULL && strcmp(e, "1.21.11") == 0, "Escape plain passthrough");
        free(e);
    }

    {
        char *e = Json_Escape("1.0\"x");
        CHECK(e != NULL && strcmp(e, "1.0\\\"x") == 0, "Escape double quote");
        free(e);
    }

    {
        char *e = Json_Escape("a\\b");
        CHECK(e != NULL && strcmp(e, "a\\\\b") == 0, "Escape backslash");
        free(e);
    }

    {
        char *e = Json_Escape("x\ny\tz\r");
        CHECK(e != NULL && strcmp(e, "x\\ny\\tz\\r") == 0, "Escape short controls");
        free(e);
    }

    {
        char *e = Json_Escape("\x01");
        CHECK(e != NULL && strcmp(e, "\\u0001") == 0, "Escape control as \\u00XX");
        free(e);
    }

    {
        char *e = Json_Escape("");
        CHECK(e != NULL && strcmp(e, "") == 0, "Escape empty string");
        free(e);
    }

    {
        char *e = Json_Escape("\xe4\xbd\xa0\xe5\xa5\xbd");
        CHECK(e != NULL && strcmp(e, "\xe4\xbd\xa0\xe5\xa5\xbd") == 0,
              "Escape UTF-8 passthrough");
        free(e);
    }

    {
        char *e = Json_Escape("a\"b\\c\nd\te\x01");
        CHECK(e != NULL && strcmp(e, "a\\\"b\\\\c\\nd\\te\\u0001") == 0,
              "Escape combined");
        free(e);
    }

#ifndef __APPLE__
    {
        malloc_calls = 0;
        malloc_fail_at = 0;
        CHECK(Json_Escape("x") == NULL, "Escape malloc fail -> NULL");
        malloc_fail_at = -1;
    }
#endif

    return failures ? 1 : 0;
}
