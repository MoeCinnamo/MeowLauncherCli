/*
 * users.json persistence. All functions here operate on the simple shape
 * this module writes: {"users":[{...}, ...]}.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/private.h"

static const char *home_dir(void)
{
#ifdef _WIN32
    return getenv("USERPROFILE");
#else
    return getenv("HOME");
#endif
}

int usersfile_path(char *buf, size_t bufsz)
{
    const char *home = home_dir();
    if (home == NULL) {
        return -1;
    }
#ifdef _WIN32
    return snprintf(buf, bufsz, "%s\\.meowlaunch\\data\\users.json", home) < 0;
#else
    return snprintf(buf, bufsz, "%s/.meowlaunch/data/users.json", home) < 0;
#endif
}

char *usersfile_load(const char *path)
{
    FILE *f = fopen(path, "rb");
    long n;
    char *buf;
    if (f == NULL) {
        return strdup("{\"users\":[]}");
    }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = malloc((size_t)n + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int usersfile_save(const char *path, const char *buf)
{
    char dir[1024];
    FILE *out;
    snprintf(dir, sizeof dir, "%s", path);
    char *slash = strrchr(dir, '/');
#ifdef _WIN32
    if (slash == NULL) slash = strrchr(dir, '\\');
#endif
    if (slash) *slash = '\0';
#ifndef _WIN32
    {
        char mk[1200];
        snprintf(mk, sizeof mk, "mkdir -p \"%s\" 2>/dev/null", dir);
        system(mk);
    }
#endif
    out = fopen(path, "wb");
    if (out == NULL) {
        return -1;
    }
    fputs(buf, out);
    fclose(out);
    return 0;
}

static int name_exists(const char *buf, const char *name)
{
    char needle[128];
    snprintf(needle, sizeof needle, "\"name\":\"%s\"", name);
    return strstr(buf, needle) != NULL;
}

int usersfile_insert(char **buf, const char *name, const char *uuid)
{
    size_t len = strlen(*buf);
    char *close, *np;

    if (name_exists(*buf, name)) {
        return -1;
    }
    np = realloc(*buf, len + 256);
    if (np == NULL) {
        return -1;
    }
    *buf = np;

    close = strrchr(*buf, ']');
    if (close == NULL) {
        return -1;
    }
    size_t tail = strlen(close);
    memmove(close + 64, close, tail + 1);
    snprintf(close, 64,
             ",{\"name\":\"%s\",\"type\":\"local\",\"uuid\":\"%s\"}",
             name, uuid);
    return 0;
}

int usersfile_remove(char **buf, const char *name)
{
    char needle[128];
    char *start, *end;
    snprintf(needle, sizeof needle, "{\"name\":\"%s\"", name);
    start = strstr(*buf, needle);
    if (start == NULL) {
        return -1;
    }
    end = strchr(start, '}');
    if (end == NULL) {
        return -1;
    }
    end++;
    if (start > *buf && *(start - 1) == ',') {
        start--;
    }
    size_t tail = strlen(end);
    memmove(start, end, tail + 1);
    return 0;
}
