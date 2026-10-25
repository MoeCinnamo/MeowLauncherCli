/*
 * Unit tests for internal/game.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "internal/game/game.c"
#include "internal/paths/paths.c"
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

    return failures ? 1 : 0;
}
