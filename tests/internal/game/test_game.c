/*
 * Unit tests for internal/game.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "internal/game/game.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    CHECK(Game_Switch("1.21.11") == 0, "Game_Switch writes");
    CHECK(Game_Current() == 0, "Game_Current reads back");
    CHECK(Game_Show() == 0, "Game_Show runs");

    /* Missing config: Game_Current prints (none). */
    remove(".meowlaunch/config.json");
    CHECK(Game_Current() == 0, "Game_Current missing config");

    /* Missing versions dir: Game_Show prints (no versions directory). */
    CHECK(Game_Show() == 0, "Game_Show missing dir");

    return failures ? 1 : 0;
}
