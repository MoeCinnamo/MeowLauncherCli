/*
 * Unit tests for internal/json/json_walk.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json_walk.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    {
        const char *j = "{\"help\":{\"a\":\"b\"}}";
        const char *end;
        const char *o = Json_FindObject(j, "help", &end);
        CHECK(o != NULL && *o == '{' && end[-1] == '}', "FindObject hit");
    }

    {
        const char *j = "{\"help\":{}}";
        const char *end;
        CHECK(Json_FindObject(j, "nope", &end) == NULL, "FindObject miss");
    }

    {
        const char *end;
        CHECK(Json_FindObject(NULL, "x", &end) == NULL, "FindObject NULL json");
    }

    {
        const char *obj = "{\"a\":\"hello\"}";
        size_t len;
        const char *v = Json_FindValue(obj, obj + strlen(obj), "a", &len);
        CHECK(v != NULL && len == 5 && strncmp(v, "hello", 5) == 0,
              "FindValue hit");
    }

    {
        const char *obj = "{\"a\" 123}";
        size_t len;
        CHECK(Json_FindValue(obj, obj + strlen(obj), "a", &len) == NULL,
              "FindValue bad colon -> NULL");
    }

    /* Nested object: brace counting skips inner closes. */
    {
        const char *j = "{\"a\":{\"b\":{\"c\":1}},\"d\":2}";
        const char *end;
        const char *o = Json_FindObject(j, "a", &end);
        CHECK(o != NULL, "FindObject nested hit");
    }

    /* skip_ws: leading whitespace. */
    {
        const char *j = "  \n\t{  \"k\" : \"v\" }";
        size_t len;
        const char *v = Json_FindValue(j, j + strlen(j), "k", &len);
        CHECK(v != NULL && len == 1 && v[0] == 'v', "FindValue skips ws");
    }

    /* Key too long -> NULL. */
    {
        const char *j = "{\"x\":\"y\"}";
        const char *end;
        char longkey[200];
        memset(longkey, 'a', sizeof longkey - 1);
        longkey[sizeof longkey - 1] = '\0';
        CHECK(Json_FindObject(j, longkey, &end) == NULL, "FindObject key too long");
        size_t len;
        CHECK(Json_FindValue(j, j + strlen(j), longkey, &len) == NULL,
              "FindValue key too long");
    }

    /* Unterminated string -> NULL. */
    {
        const char *j = "{\"a\":\"oops";
        size_t len;
        CHECK(Json_FindValue(j, j + strlen(j), "a", &len) == NULL,
              "FindValue unterminated string");
    }

    return failures ? 1 : 0;
}
