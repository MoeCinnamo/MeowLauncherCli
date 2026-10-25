/*
 * Runtime localization.
 *
 * Language archives (JSON files under lang/) are scanned and embedded into
 * the binary at build time by scripts/build.sh into a generated
 * builtin_langs[] table. The active archive is chosen from the system locale
 * (LANG); the first embedded archive (English) is the fallback.
 *
 * JSON walking is delegated to internal/json.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *strcasestr(const char *hay, const char *needle)
{
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    for (; *hay; hay++) {
        size_t i;
        for (i = 0; i < nlen; i++) {
            if (tolower((unsigned char)hay[i]) != tolower((unsigned char)needle[i])) break;
        }
        if (i == nlen) return (char *)hay;
    }
    return NULL;
}

#include "internal/i18n/i18n.h"
#include "internal/i18n/gen/lang_embedded.h"
#include "internal/json/json.h"

/* Active archive, selected in I18n_Init(). */
static const char *active_json;

/* English archive used as lookup fallback; NULL when English was not
 * compiled in. */
static const char *fallback_json;

/* Rendered lookup result; single-threaded CLI, one static slot suffices. */
static char out_buf[256];

void I18n_Init(void)
{
    /* builtin_langs[0] is the fallback archive (English when present);
     * every later entry matches a locale prefix. */
    active_json = builtin_langs[0].json;
    fallback_json = builtin_langs[0].json;

    const char *lang = getenv("LANG");
    if (lang == NULL || lang[0] == '\0') {
        return;
    }

    /* Require UTF-8 or C/POSIX; anything else (GBK, Shift_JIS, etc.)
     * falls back to English to avoid mojibake on Windows. */
    const char *dot = strchr(lang, '.');
    if (dot != NULL) {
        if (strcasestr(dot, "utf-8") == NULL && strcasestr(dot, "utf8") == NULL) {
            return;
        }
    }

    for (size_t i = 1; i < BUILTIN_LANGS_COUNT; i++) {
        size_t plen = strlen(builtin_langs[i].prefix);
        if (strncmp(lang, builtin_langs[i].prefix, plen) != 0) continue;
        /* Boundary: next char after prefix must be '_', '-' or '.' or end. */
        char next = lang[plen];
        if (next != '\0' && next != '_' && next != '-' && next != '.') continue;
        active_json = builtin_langs[i].json;
        break;
    }
}

const char *I18n(const char *path)
{
    /* Walk dotted path: each segment before the last is an object, the
     * last segment is a quoted string value. */
    const char *orig = path;
    const char *cur = active_json;
    const char *cur_end = active_json + strlen(active_json);
    char seg[32];
    size_t seg_len;

    if (strchr(path, '.') == NULL) {
        snprintf(out_buf, sizeof out_buf, "%s", path);
        return out_buf;
    }

    while (*path) {
        const char *dot = strchr(path, '.');
        if (dot == NULL) {
            seg_len = strlen(path);
            snprintf(seg, sizeof seg, "%.*s", (int)seg_len, path);
            size_t vlen;
            const char *v = Json_FindValue(cur, cur_end, seg, &vlen);
            if (v != NULL) {
                snprintf(out_buf, sizeof out_buf, "%.*s", (int)vlen, v);
                return out_buf;
            }
            break;
        }
        seg_len = (size_t)(dot - path);
        snprintf(seg, sizeof seg, "%.*s", (int)seg_len, path);
        const char *obj_end;
        const char *body = Json_FindObject(cur, seg, &obj_end);
        if (body == NULL) {
            break;
        }
        cur = body;
        cur_end = obj_end;
        path = dot + 1;
    }

    /* Miss: fall back to English, unless already on it. */
    if (fallback_json != NULL && active_json != fallback_json) {
        const char *saved = active_json;
        active_json = fallback_json;
        const char *result = I18n(orig);
        active_json = saved;
        return result;
    }

    snprintf(out_buf, sizeof out_buf, "[%s]", orig);
    return out_buf;
}
