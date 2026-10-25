/*
 * Unit tests for internal/game.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "internal/game/game.c"
#include "internal/platform/platform.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"
#include "internal/json/json_escape.c"

#ifndef __APPLE__
/* Linker wraps: --wrap,malloc routes malloc -> __wrap_malloc. */
static int malloc_fail_at = -1;
static int malloc_calls = 0;
void *__real_malloc(size_t);
void *__wrap_malloc(size_t n)
{
    malloc_calls++;
    if (malloc_fail_at >= 0 && malloc_calls > malloc_fail_at) return NULL;
    return __real_malloc(n);
}
#endif

const char *I18n(const char *key) { return key; }

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    /* Create fake .minecraft/versions/<ver> dirs next to test binary. */
#ifdef _WIN32
    _mkdir("build_test/.minecraft");
    _mkdir("build_test/.minecraft/versions");
    _mkdir("build_test/.minecraft/versions/1.21.11");
    _mkdir("build_test/.minecraft/versions/1.21.11-Neoforge");
#else
    mkdir("build_test/.minecraft", 0755);
    mkdir("build_test/.minecraft/versions", 0755);
    mkdir("build_test/.minecraft/versions/1.21.11", 0755);
    mkdir("build_test/.minecraft/versions/1.21.11-Neoforge", 0755);
#endif

    CHECK(Game_Switch("1.21.11") == 0, "Game_Switch writes");
    CHECK(Game_Current() == 0, "Game_Current reads back");
    CHECK(Game_Show() == 0, "Game_Show lists versions");

    /* JSON injection: quotes must be escaped on write, not injected. */
    CHECK(Game_Switch("1.21.11\"Neoforge") == 0, "Game_Switch escapes quotes");
    {
        char raw[256];
        size_t got = 0;
        FILE *f = fopen("build_test/.meowlaunch/config.json", "rb");
        if (f != NULL) {
            got = fread(raw, 1, sizeof raw - 1, f);
            fclose(f);
        }
        raw[got] = '\0';
        CHECK(strcmp(raw, "{\"game_version\":\"1.21.11\\\"Neoforge\"}") == 0,
              "Game_Switch writes escaped json");
    }
    {
        char *j = Json_Load("build_test/.meowlaunch/config.json");
        size_t vlen;
        const char *v = j != NULL
            ? Json_FindValue(j, j + strlen(j), "game_version", &vlen)
            : NULL;
        CHECK(v != NULL && vlen == 17
              && memcmp(v, "1.21.11\\\"Neoforge", 17) == 0,
              "Game_Switch escaped value reads back fully");
        free(j);
    }

    /* Missing config: Game_Current prints (none). */
    remove("build_test/.meowlaunch/config.json");
    CHECK(Game_Current() == 0, "Game_Current missing config");

    /* Missing config with malformed JSON: Game_Current prints (none). */
#ifdef _WIN32
    _mkdir("build_test/.meowlaunch");
#else
    mkdir("build_test/.meowlaunch", 0755);
#endif
    FILE *f = fopen("build_test/.meowlaunch/config.json", "w");
    fputs("not json", f);
    fclose(f);
    CHECK(Game_Current() == 0, "Game_Current malformed json");
    remove("build_test/.meowlaunch/config.json");

    /* Missing versions dir: Game_Show prints (no versions directory). */
    rename("build_test/.minecraft/versions", "build_test/.minecraft/versions_bak");
    CHECK(Game_Show() == 0, "Game_Show missing versions dir");
    rename("build_test/.minecraft/versions_bak", "build_test/.minecraft/versions");

    /* Json_Save failure: .meowlaunch path blocked by a file -> rc != 0. */
#ifdef _WIN32
    system("rmdir /s /q build_test\\.meowlaunch >nul 2>&1");
#else
    system("rm -rf build_test/.meowlaunch");
#endif
    {
        FILE *f = fopen("build_test/.meowlaunch", "w");
        if (f != NULL) {
            fputs("x", f);
            fclose(f);
        }
    }
    CHECK(Game_Switch("blocked") == 1, "Game_Switch save fail -> 1");
    remove("build_test/.meowlaunch");

#ifndef __APPLE__
    /* malloc failure in Json_Escape: esc == NULL -> Game_Switch returns 1. */
    malloc_calls = 0;
    malloc_fail_at = 0;
    CHECK(Game_Switch("oom") == 1, "Game_Switch escape malloc fail -> 1");
    malloc_fail_at = -1;

    /* malloc failure for the config buffer: buf == NULL -> Game_Switch
     * returns 1, exercising the free(esc) + return cleanup. */
    malloc_calls = 0;
    malloc_fail_at = 1;
    CHECK(Game_Switch("oom") == 1, "Game_Switch buf malloc fail -> 1");
    malloc_fail_at = -1;
#endif

    return failures ? 1 : 0;
}
