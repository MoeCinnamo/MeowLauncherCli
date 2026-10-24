/*
 * Runtime localization.
 *
 * Language archives (lang/*.json) are embedded into the binary at build
 * time by scripts/build.sh as LANG_JSON_* macros. The active archive
 * is chosen from the system locale (LANG); English is the fallback.
 *
 * The archive format is intentionally minimal: nested objects whose values
 * are plain JSON strings, e.g. {"help": {"tagline": "..."}}. No full JSON
 * parser is needed for this controlled subset.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/i18n/i18n.h"
#include "internal/i18n/gen/lang_embedded.h"

/* Active archive, selected in I18n_Init(). */
static const char *active_json;

/* Rendered lookup result; single-threaded CLI, one static slot suffices. */
static char out_buf[256];

static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    return p;
}

/*
 * Return the body of the "key": { ... } object, or NULL.
 * *end receives the position of the closing brace. The archives keep
 * braces out of string values, so a plain strchr bounds the object.
 */
static const char *find_object(const char *json, const char *key,
                               const char **end)
{
    char pat[64];
    const char *p;

    snprintf(pat, sizeof pat, "\"%s\"", key);
    p = strstr(json, pat);
    if (p == NULL) {
        return NULL;
    }
    p = skip_ws(p + strlen(pat));
    if (*p != ':') {
        return NULL;
    }
    p = skip_ws(p + 1);
    if (*p != '{') {
        return NULL;
    }
    *end = strchr(p + 1, '}');
    return *end ? p + 1 : NULL;
}

/* Find "key": "value" inside [start, end). Returns the value start and
 * sets *len; NULL when absent. Values must not contain quotes. */
static const char *find_value(const char *start, const char *end,
                              const char *key, size_t *len)
{
    char pat[64];
    size_t pat_len;
    const char *p = start;

    snprintf(pat, sizeof pat, "\"%s\"", key);
    pat_len = strlen(pat);

    while (p + pat_len <= end) {
        const char *q = p;
        const char *hit = NULL;

        for (; q + pat_len <= end; q++) {
            if (memcmp(q, pat, pat_len) == 0) {
                hit = q;
                break;
            }
        }
        if (hit == NULL) {
            return NULL;
        }

        p = skip_ws(hit + pat_len);
        if (*p != ':') {
            p = hit + 1;
            continue;
        }
        p = skip_ws(p + 1);
        if (*p != '"') {
            p = hit + 1;
            continue;
        }
        p++;
        const char *vstart = p;
        while (p < end && *p != '"') {
            p++;
        }
        *len = (size_t)(p - vstart);
        return vstart;
    }
    return NULL;
}

void I18n_Init(void)
{
    const char *lang = getenv("LANG");

#ifdef LANG_JSON_EN
    /* Multi-language build: English is the fallback; the system locale
     * overrides it only when that language was compiled in. */
    active_json = LANG_JSON_EN;
#else
    /* Single-language build: no English fallback exists, so use the one
     * language compiled in regardless of the system locale. */
#  ifdef LANG_JSON_ZH_TW
    active_json = LANG_JSON_ZH_TW;
#  elif defined(LANG_JSON_JA_JP)
    active_json = LANG_JSON_JA_JP;
#  else
    active_json = "";
#  endif
#endif

#ifdef LANG_JSON_ZH_TW
    if (lang != NULL && strncmp(lang, "zh", 2) == 0) {
        active_json = LANG_JSON_ZH_TW;
    }
#endif

#ifdef LANG_JSON_JA_JP
    if (lang != NULL && strncmp(lang, "ja", 2) == 0) {
        active_json = LANG_JSON_JA_JP;
    }
#endif
}

const char *I18n(const char *path)
{
    const char *dot = strchr(path, '.');
    char section[32], key[40];
    const char *obj_end, *body, *val;
    size_t len;

    if (dot == NULL) {
        snprintf(out_buf, sizeof out_buf, "%s", path);
        return out_buf;
    }

    snprintf(section, sizeof section, "%.*s", (int)(dot - path), path);
    snprintf(key, sizeof key, "%s", dot + 1);

    body = find_object(active_json, section, &obj_end);
    if (body != NULL) {
        val = find_value(body, obj_end, key, &len);
        if (val != NULL) {
            snprintf(out_buf, sizeof out_buf, "%.*s", (int)len, val);
            return out_buf;
        }
    }

    /* Miss: fall back to English, unless already on it. */
#ifdef LANG_JSON_EN
    if (active_json != LANG_JSON_EN) {
        const char *saved = active_json;
        active_json = LANG_JSON_EN;
        const char *result = I18n(path);
        active_json = saved;
        return result;
    }
#endif

    snprintf(out_buf, sizeof out_buf, "[%s]", path);
    return out_buf;
}
