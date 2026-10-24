/*
 * Unit tests for internal/i18n.
 *
 * Built by scripts/test.sh together with internal/i18n/i18n.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/i18n/i18n.h"

static int failures = 0;

#define CHECK(cond, name)                                              \
    do {                                                               \
        if (cond) { printf("[PASS] %s\n", name); }                     \
        else { printf("[FAIL] %s\n", name); failures++; }              \
    } while (0)

int main(void)
{
    /* English default. */
    setenv("LANG", "C", 1);
    I18n_Init();
    CHECK(strcmp(I18n("help.usage"), "Usage: meowlaunchcli [command] [arguments]") == 0,
          "english help.usage");

    /* Unknown locale falls back to English. */
    setenv("LANG", "fr_FR.UTF-8", 1);
    I18n_Init();
    CHECK(strncmp(I18n("help.usage"), "Usage:", 6) == 0,
          "fr locale falls back to english");

    /* zh_TW selects the zh-TW archive. */
    setenv("LANG", "zh_TW.UTF-8", 1);
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "指令") != NULL,
          "zh_TW selects chinese");

    /* ja_JP selects the Japanese archive. */
    setenv("LANG", "ja_JP.UTF-8", 1);
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "コマンド") != NULL,
          "ja_JP selects japanese");

    /* Missing key renders a [key] placeholder. */
    CHECK(strcmp(I18n("help.nope"), "[help.nope]") == 0,
          "missing key placeholder");

    return failures ? 1 : 0;
}
