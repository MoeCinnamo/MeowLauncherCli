/*
 * Shared helpers for internal/config.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json.h"
#include "internal/paths/paths.h"
#include "internal/config/config.h"

int config_path(char *buf, size_t bufsz)
{
    char dir[1024];
    if (ExeDir(dir, sizeof dir) != 0) return -1;
#ifdef _WIN32
    return snprintf(buf, bufsz, "%s\\.meowlaunch\\config.json", dir) < 0;
#else
    return snprintf(buf, bufsz, "%s/.meowlaunch/config.json", dir) < 0;
#endif
}

void config_read_str(const char *key, const char *def, char *out, size_t outsz)
{
    char path[1024];
    out[0] = '\0';
    if (config_path(path, sizeof path) != 0) { snprintf(out, outsz, "%s", def); return; }
    char *j = Json_Load(path);
    if (j == NULL) { snprintf(out, outsz, "%s", def); return; }
    size_t len;
    const char *v = Json_FindValue(j, j + strlen(j), key, &len);
    if (v != NULL) snprintf(out, outsz, "%.*s", (int)len, v);
    else snprintf(out, outsz, "%s", def);
    free(j);
}

int config_write_json(const char *obj)
{
    char path[1024];
    if (config_path(path, sizeof path) != 0) return 1;
    return Json_Save(path, obj) != 0;
}
