/*
 * Runtime localization.
 *
 * Language archives (JSON files under lang/) are scanned and embedded into
 * the binary at build time by scripts/build.sh into a generated
 * builtin_langs[] table. The active archive is chosen from the system locale
 * (LANG); the first embedded archive (English) is the fallback.
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

/* English archive used as lookup fallback; NULL when English was not
 * compiled in. */
static const char *fallback_json;

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
    /* builtin_langs[0] is the fallback archive (English when present);
     * every later entry matches a locale prefix. */
    active_json = builtin_langs[0].json;
    fallback_json = builtin_langs[0].json;

    const char *lang = getenv("LANG");
    if (lang == NULL) {
        return;
    }
    for (size_t i = 1; i < BUILTIN_LANGS_COUNT; i++) {
        size_t plen = strlen(builtin_langs[i].prefix);
        if (strncmp(lang, builtin_langs[i].prefix, plen) == 0) {
            active_json = builtin_langs[i].json;
            break;
        }
    }
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

    /* Miss: fall back to English, unless already on it. Comparing the
     * pointer variables (not the literals) keeps the comparison defined. */
    if (fallback_json != NULL && active_json != fallback_json) {
        const char *saved = active_json;
        active_json = fallback_json;
        const char *result = I18n(path);
        active_json = saved;
        return result;
    }

    snprintf(out_buf, sizeof out_buf, "[%s]", path);
    return out_buf;
}
