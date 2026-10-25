/*
 * users.json persistence. All functions here operate on the simple shape
 * this module writes: {"users":[{...}, ...]}.
 *
 * Data lives next to the executable: <exedir>/data/users.json.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <libgen.h>
#else
#include <unistd.h>
#include <libgen.h>
#endif

#include "internal/json/json.h"
#include "internal/account/private.h"
#include "internal/paths/paths.h"

int usersfile_path(char *buf, size_t bufsz)
{
    char dir[1024];
    if (ExeDir(dir, sizeof dir) != 0) {
        return -1;
    }
#ifdef _WIN32
    return snprintf(buf, bufsz, "%s\\.meowlaunch\\data\\users.json", dir) < 0;
#else
    return snprintf(buf, bufsz, "%s/.meowlaunch/data/users.json", dir) < 0;
#endif
}

char *usersfile_load(const char *path)
{
    char *r = Json_Load(path);
    if (r == NULL) {
        return strdup("{\"users\":[]}");
    }
    return r;
}

int usersfile_save(const char *path, const char *buf)
{
    return Json_Save(path, buf);
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
