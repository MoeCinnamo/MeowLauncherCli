/*
 * Unit tests for internal/game/help.c.
 */

#include <stdio.h>
#include <string.h>

#include "internal/i18n/i18n.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"
#include "internal/game/help.c"

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    I18n_Init();
    Game_Help();
    CHECK(1, "Game_Help runs");
    return failures ? 1 : 0;
}
