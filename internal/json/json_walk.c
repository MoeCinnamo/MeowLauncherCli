/*
 * Minimal JSON string walker: Json_FindObject / Json_FindValue.
 * Handles flat objects and quoted string values only.
 */

#include <stdio.h>
#include <string.h>

#include "internal/json/json.h"

static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    return p;
}

const char *Json_FindObject(const char *json, const char *key,
                            const char **end)
{
    char pat[128];
    const char *p;
    size_t pat_len;

    if (json == NULL) {
        return NULL;
    }
    snprintf(pat, sizeof pat, "\"%s\"", key);
    pat_len = strlen(pat);

    p = strstr(json, pat);
    if (p == NULL) {
        return NULL;
    }
    p = skip_ws(p + pat_len);
    if (*p != ':') {
        return NULL;
    }
    p = skip_ws(p + 1);
    if (*p != '{') {
        return NULL;
    }
    /* Count braces to find the matching close, accounting for nesting. */
    int depth = 1;
    const char *q = p + 1;
    while (*q && depth > 0) {
        if (*q == '{') depth++;
        else if (*q == '}') depth--;
        q++;
    }
    if (depth != 0) {
        return NULL;
    }
    *end = q;
    return p;
}

const char *Json_FindValue(const char *start, const char *end,
                           const char *key, size_t *vlen)
{
    const char *p = start;
    size_t pat_len = strlen(key);
    char pat[128];

    if (pat_len + 2 >= sizeof pat) {
        return NULL;
    }
    snprintf(pat, sizeof pat, "\"%s\"", key);
    pat_len += 2;

    while (p + pat_len <= end) {
        const char *hit = NULL;
        const char *q;
        for (q = p; q + pat_len <= end; q++) {
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
            return NULL;
        }
        p++;
        const char *vstart = p;
        while (p < end && *p != '"') {
            if (*p == '\\' && p + 1 < end) {
                p += 2; /* skip the escaped character (e.g. \" or \\) */
            } else {
                p++;
            }
        }
        if (p >= end) {
            return NULL;
        }
        *vlen = (size_t)(p - vstart);
        return vstart;
    }
    return NULL;
}
