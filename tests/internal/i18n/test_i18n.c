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
#include "internal/json/json_walk.c"

/* i18n used to have its own walkers; now it delegates to Json_*. */
#define find_object Json_FindObject
#define find_value  Json_FindValue

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

    /* Nested lookup: error.unknown_command. */
    set_lang("C");
    I18n_Init();
    CHECK(strncmp(I18n("error.unknown_command"), "Unknown command", 15) == 0,
          "error.unknown_command resolves");

    /* Mid-walk miss: error.nope.x — error exists, nope does not. */
    CHECK(strcmp(I18n("error.nope.x"), "[error.nope.x]") == 0,
          "nested mid-walk miss -> placeholder");

    /* --- skip_ws covered in json_walk unit tests --- */
    (void)0;

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

    /* --- find_object: missing closing brace (strchr returns NULL) --- */
    {
        const char *json = "{\"help\":{\"a\":\"b\"";
        const char *end;
        CHECK(find_object(json, "help", &end) == NULL,
              "find_object unterminated object -> NULL");
    }

    /* --- find_value: outer loop never enters (line 111) --- */
    {
        const char *obj = "{}";
        size_t len;
        /* end is shorter than the pattern, so the while condition fails at
         * once and we reach the trailing return NULL. */
        CHECK(find_value(obj, obj + 1, "a", &len) == NULL,
              "find_value range exhausted -> NULL");
    }

    /* --- I18n_Init: LANG unset returns early (line 123) --- */
    {
#ifdef _WIN32
        putenv("LANG=");
#else
        unsetenv("LANG");
#endif
        I18n_Init();
        CHECK(active_json == builtin_langs[0].json,
              "unset LANG stays on fallback");
    }

    /* --- I18n: path without dot returns the path verbatim (lines 142-143) --- */
    set_lang("C");
    I18n_Init();
    CHECK(strcmp(I18n("nodot"), "nodot") == 0,
          "I18n no-dot path echoed verbatim");

    /* --- I18n: fallback recursion when active archive misses a key (line 160) --- */
    /* On the zh archive, ask for a key that zh lacks. The fallback branch
     * fires (active != fallback), recurses into en; en also lacks it, so
     * the [key] placeholder comes back. The recursion path itself is what
     * we exercise. */
    set_lang("zh_TW.UTF-8");
    I18n_Init();
    CHECK(strcmp(I18n("help.totally_absent"), "[help.totally_absent]") == 0,
          "zh miss recurses to en fallback then placeholder");

    /* --- find_value: multi-char value drives the scan loop (line 105) --- */
    {
        const char *obj = "{\"a\":\"hello world\"}";
        size_t len;
        const char *v = find_value(obj, obj + strlen(obj), "a", &len);
        CHECK(v != NULL && len == 11 && strncmp(v, "hello world", 11) == 0,
              "find_value multi-char value scanned to closing quote");
    }

    /* --- I18n line 160: active == fallback, no recursion --- */
    set_lang("C");
    I18n_Init();
    CHECK(active_json == fallback_json, "en build: active == fallback");
    /* Missing key on en: fallback branch is skipped, placeholder returns. */
    CHECK(strcmp(I18n("help.nope"), "[help.nope]") == 0,
          "en missing key returns placeholder without recursion");

    /* --- I18n line 160: active != fallback but key exists locally --- */
    /* On zh, ask for a key zh actually has: the condition is true but the
     * local lookup succeeds, so the fallback branch is not entered. */
    set_lang("zh_TW.UTF-8");
    I18n_Init();
    CHECK(active_json != fallback_json, "zh build: active != fallback");
    CHECK(strstr(I18n("help.usage"), "使用") != NULL ||
          strstr(I18n("help.usage"), "用法") != NULL ||
          strstr(I18n("help.usage"), "指令") != NULL,
          "zh key resolved locally, no fallback needed");

    return failures ? 1 : 0;
}
