/*
 * Unit tests for internal/i18n.
 *
 * Built by scripts/test.sh together with internal/i18n/i18n.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/i18n/i18n.h"

/* setenv() is POSIX and absent from the Windows CRT; emulate it via
 * putenv() there. */
#ifdef _WIN32
static void set_lang(const char *value)
{
    static char buf[64];
    snprintf(buf, sizeof buf, "LANG=%s", value);
    putenv(buf);
}
#else
static void set_lang(const char *value)
{
    setenv("LANG", value, 1);
}
#endif

static int failures = 0;

#define CHECK(cond, name)                                              \
    do {                                                               \
        if (cond) { printf("[PASS] %s\n", name); }                     \
        else { printf("[FAIL] %s\n", name); failures++; }              \
    } while (0)

int main(void)
{
    /* English default. */
    set_lang("C");
    I18n_Init();
    CHECK(strcmp(I18n("help.usage"), "Usage: meowlaunchcli [command] [arguments]") == 0,
          "english help.usage");

    /* Unknown locale falls back to English. */
    set_lang("fr_FR.UTF-8");
    I18n_Init();
    CHECK(strncmp(I18n("help.usage"), "Usage:", 6) == 0,
          "fr locale falls back to english");

    /* zh_TW selects the zh-TW archive. */
    set_lang("zh_TW.UTF-8");
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "指令") != NULL,
          "zh_TW selects chinese");

    /* ja_JP selects the Japanese archive. */
    set_lang("ja_JP.UTF-8");
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "コマンド") != NULL,
          "ja_JP selects japanese");

    /* Missing key renders a [key] placeholder. */
    CHECK(strcmp(I18n("help.nope"), "[help.nope]") == 0,
          "missing key placeholder");

    return failures ? 1 : 0;
}
