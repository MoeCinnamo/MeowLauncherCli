/*
 * Unit tests for internal/i18n — covers both public lookup and the static
 * JSON-walker branches (skip_ws, find_object, find_value).
 *
 * We include i18n.c directly so the static helpers become reachable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/i18n/i18n.c"

static int failures = 0;

#define CHECK(cond, name)                                              \
    do {                                                               \
        if (cond) { printf("[PASS] %s\n", name); }                     \
        else { printf("[FAIL] %s\n", name); failures++; }              \
    } while (0)

static void set_lang(const char *value)
{
#ifdef _WIN32
    static char buf[64];
    snprintf(buf, sizeof buf, "LANG=%s", value);
    putenv(buf);
#else
    setenv("LANG", value, 1);
#endif
}

int main(void)
{
    /* --- Public locale/lookup path --- */
    set_lang("C");
    I18n_Init();
    CHECK(strcmp(I18n("help.usage"), "Usage: meowlaunchcli [command] [arguments]") == 0,
          "english help.usage");

    set_lang("fr_FR.UTF-8");
    I18n_Init();
    CHECK(strncmp(I18n("help.usage"), "Usage:", 6) == 0,
          "fr locale falls back to english");

    set_lang("zh_TW.UTF-8");
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "指令") != NULL,
          "zh_TW selects chinese");

    set_lang("ja_JP.UTF-8");
    I18n_Init();
    CHECK(strstr(I18n("help.commands_header"), "コマンド") != NULL,
          "ja_JP selects japanese");

    CHECK(strcmp(I18n("help.nope"), "[help.nope]") == 0,
          "missing key placeholder");

    /* --- skip_ws: eats every whitespace kind --- */
    {
        const char *s = "  \t\n\r x";
        CHECK(*skip_ws(s) == 'x', "skip_ws eats spaces/tabs/newlines/CR");
        CHECK(*skip_ws("x") == 'x', "skip_ws on non-whitespace");
    }

    /* --- find_object: section hits / missing --- */
    {
        const char *json = "{\"help\":{\"a\":\"b\"}}";
        const char *end;
        CHECK(find_object(json, "help", &end) != NULL, "find_object hits section");
        CHECK(find_object(json, "nope", &end) == NULL, "find_object missing section -> NULL");
    }

    /* --- find_object: missing colon --- */
    {
        const char *json = "{\"help\" {\"a\":\"b\"}}";
        const char *end;
        CHECK(find_object(json, "help", &end) == NULL, "find_object bad colon -> NULL");
    }

    /* --- find_object: missing brace --- */
    {
        const char *json = "{\"help\": 123}";
        const char *end;
        CHECK(find_object(json, "help", &end) == NULL, "find_object bad brace -> NULL");
    }

    /* --- find_value: missing key --- */
    {
        const char *obj = "{\"a\":\"b\"}";
        size_t len;
        CHECK(find_value(obj, obj + strlen(obj), "zz", &len) == NULL,
              "find_value missing key -> NULL");
    }

    /* --- find_value: colon mismatch (key not followed by :) --- */
    {
        const char *obj = "{\"a\" 123}";
        size_t len;
        CHECK(find_value(obj, obj + strlen(obj), "a", &len) == NULL,
              "find_value missing colon -> NULL");
    }

    /* --- find_value: value not quoted --- */
    {
        const char *obj = "{\"a\": 123}";
        size_t len;
        CHECK(find_value(obj, obj + strlen(obj), "a", &len) == NULL,
              "find_value unquoted value -> NULL");
    }

    /* --- find_value: normal hit --- */
    {
        const char *obj = "{\"a\":\"hello\"}";
        size_t len;
        const char *v = find_value(obj, obj + strlen(obj), "a", &len);
        CHECK(v != NULL && len == 5 && strncmp(v, "hello", 5) == 0,
              "find_value normal hit");
    }

    return failures ? 1 : 0;
}
