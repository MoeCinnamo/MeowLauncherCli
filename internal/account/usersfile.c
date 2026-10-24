/*
 * users.json persistence. All functions here operate on the simple shape
 * this module writes: {"users":[{...}, ...]}.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#define MKDIR(p) mkdir((p), 0755)
#endif

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
    char tmp[1024];
    FILE *out;
    char *p;

    /* Recursively mkdir every leading directory component. */
    snprintf(tmp, sizeof tmp, "%s", path);
    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = '\0';
            MKDIR(tmp);
            *p = c;
        }
    }

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
    char *close;
    size_t head_len, tail_len, new_len;
    char entry[200];
    int entry_len;
    char *nb;

    if (name_exists(*buf, name)) {
        return -1;
    }
    close = strrchr(*buf, ']');
    if (close == NULL) {
        return -1;
    }
    head_len = (size_t)(close - *buf);
    tail_len = strlen(close);
    /* No leading comma when the array is still empty ("...[]"). */
    entry_len = snprintf(entry, sizeof entry,
                         "%s{\"name\":\"%s\",\"type\":\"local\",\"uuid\":\"%s\"}",
                         (head_len > 0 && (*buf)[head_len - 1] == '[') ? "" : ",",
                         name, uuid);
    new_len = head_len + (size_t)entry_len + tail_len + 1;
    nb = malloc(new_len);
    if (nb == NULL) {
        return -1;
    }
    memcpy(nb, *buf, head_len);
    memcpy(nb + head_len, entry, (size_t)entry_len);
    strcpy(nb + head_len + (size_t)entry_len, close);
    free(*buf);
    *buf = nb;
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
