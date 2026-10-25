/*
 * Unit tests for internal/platform/platform.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/platform/platform.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    {
        char buf[1024];
        CHECK(Plat_ExeDir(buf, sizeof buf) == 0 && buf[0] != '\0',
              "ExeDir resolves");
    }

    /* Plat_Join: native separator, / in rel normalized. */
    {
        char buf[128];
        CHECK(Plat_Join(buf, sizeof buf, "base", "a/b") == 0,
              "Join succeeds");
        CHECK(strcmp(buf, "base/a/b") == 0 || strcmp(buf, "base\\a\\b") == 0,
              "Join uses platform separator");
    }

    /* Plat_Join: small buffer -> nonzero. */
    {
        char buf[8];
        CHECK(Plat_Join(buf, sizeof buf, "base", "a/b") != 0,
              "Join truncates -> nonzero");
    }

    /* Plat_Mkdir + Plat_IsDir (clean slate first so re-runs pass). */
    remove("build_test/plat_tmp/f.txt"); /* file first: rmdir needs empty dir */
    Plat_Rmdir("build_test/plat_tmp");   /* remove() cannot delete dirs on Windows */
    CHECK(Plat_Mkdir("build_test/plat_tmp") == 0, "Mkdir creates");
    CHECK(Plat_Mkdir("build_test/plat_tmp") != 0, "Mkdir existing -> nonzero");
    CHECK(Plat_IsDir("build_test/plat_tmp") == 1, "IsDir sees directory");
    CHECK(Plat_IsDir("build_test/plat_tmp/nope") == 0, "IsDir missing -> 0");
    {
        FILE *f = fopen("build_test/plat_tmp/f.txt", "w");
        if (f != NULL) {
            fputs("x", f);
            fclose(f);
        }
        CHECK(Plat_IsDir("build_test/plat_tmp/f.txt") == 0,
              "IsDir file -> 0");
    }

    /* Directory iteration finds the file entry. */
    {
        plat_dir_t *d = Plat_Dir_Open("build_test/plat_tmp");
        CHECK(d != NULL, "DirOpen ok");
        if (d != NULL) {
            int seen = 0;
            const char *name;
            while (Plat_Dir_Next(d, &name) > 0) {
                if (strcmp(name, "f.txt") == 0) {
                    seen = 1;
                }
            }
            Plat_Dir_Close(d);
            CHECK(seen, "Dir iteration finds f.txt");
        }
    }

    CHECK(Plat_Dir_Open("build_test/plat_tmp/nope") == NULL,
          "DirOpen missing -> NULL");

    return failures ? 1 : 0;
}
