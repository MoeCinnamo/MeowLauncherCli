/*
 * Unit tests for internal/json/json_io.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json_io.c"

static int malloc_fail_at = -1;
static int malloc_calls = 0;
void *__real_malloc(size_t);
void *__wrap_malloc(size_t n)
{
    malloc_calls++;
    if (malloc_fail_at >= 0 && malloc_calls > malloc_fail_at) return NULL;
    return __real_malloc(n);
}

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    CHECK(Json_Load("/nonexistent/___no_such_file___.json") == NULL,
          "Json_Load missing -> NULL");

    {
        system("rm -rf build_test/json_io_tmp");
        char path[256];
        snprintf(path, sizeof path, "build_test/json_io_tmp/sub/dir/data.json");
        CHECK(Json_Save(path, "{\"x\":1}") == 0, "Json_Save creates subdirs");
        char *loaded = Json_Load(path);
        CHECK(loaded != NULL && strstr(loaded, "\"x\"") != NULL,
              "Json_Load reads saved file");
        free(loaded);
    }

    /* Json_Save to an unwritable path: fopen fails -> -1. */
    CHECK(Json_Save("build_test/json_io_tmp/sub/dir/data.json/inside", "x") != 0,
          "Json_Save unwritable -> -1");

    /* malloc failure in Json_Load: buf=NULL path. */
    malloc_calls = 0;
    malloc_fail_at = 0;
    CHECK(Json_Load("build_test/json_io_tmp/sub/dir/data.json") == NULL,
          "Json_Load malloc fail -> NULL");
    malloc_fail_at = -1;

    return failures ? 1 : 0;
}
