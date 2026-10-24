/*
 * Unit tests for internal/json.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    /* FindObject hit. */
    {
        const char *j = "{\"help\":{\"a\":\"b\"}}";
        const char *end;
        const char *o = Json_FindObject(j, "help", &end);
        CHECK(o != NULL && *o == '{' && end[-1] == '}', "FindObject hit");
    }

    /* FindObject missing. */
    {
        const char *j = "{\"help\":{}}";
        const char *end;
        CHECK(Json_FindObject(j, "nope", &end) == NULL, "FindObject miss");
    }

    /* FindObject NULL. */
    {
        const char *end;
        CHECK(Json_FindObject(NULL, "x", &end) == NULL, "FindObject NULL json");
    }

    /* FindValue hit. */
    {
        const char *obj = "{\"a\":\"hello\"}";
        size_t len;
        const char *v = Json_FindValue(obj, obj + strlen(obj), "a", &len);
        CHECK(v != NULL && len == 5 && strncmp(v, "hello", 5) == 0,
              "FindValue hit");
    }

    /* FindValue colon mismatch. */
    {
        const char *obj = "{\"a\" 123}";
        size_t len;
        CHECK(Json_FindValue(obj, obj + strlen(obj), "a", &len) == NULL,
              "FindValue bad colon -> NULL");
    }

    /* Load missing file. */
    CHECK(Json_Load("/nonexistent/___no_such_file___.json") == NULL,
          "Json_Load missing -> NULL");

    return failures ? 1 : 0;
}
