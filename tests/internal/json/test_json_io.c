/*
 * Unit tests for internal/json/json_io.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json_io.c"

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

    return failures ? 1 : 0;
}
