/*
 * Current game version stored in <exedir>/.meowlaunch/config.json.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json.h"
#include "internal/i18n/i18n.h"
#include "internal/platform/platform.h"

static int config_path(char *buf, size_t bufsz)
{
    char dir[1024];
    if (Plat_ExeDir(dir, sizeof dir) != 0) return -1;
    return Plat_Join(buf, bufsz, dir, ".meowlaunch/config.json");
}

/* Set the current game version. Returns 0 on success. */
int Game_Switch(const char *version)
{
    char path[1024];
    char *esc;
    char *buf;
    size_t n;
    int rc;

    if (config_path(path, sizeof path) != 0) return 1;
    esc = Json_Escape(version);
    if (esc == NULL) return 1;
    /* Exact-size buffer: escaped form can grow up to 6x the input. */
    n = strlen(esc) + sizeof("{\"game_version\":\"\"}");
    buf = malloc(n);
    if (buf == NULL) {
        free(esc);
        return 1;
    }
    snprintf(buf, n, "{\"game_version\":\"%s\"}", esc);
    free(esc);
    rc = Json_Save(path, buf);
    free(buf);
    if (rc != 0) return 1;
    printf("%s: %s\n", I18n("game.switched"), version);
    return 0;
}

/* Print the current game version to stdout. Returns 0 on success. */
int Game_Current(void)
{
    char path[1024];
    if (config_path(path, sizeof path) != 0) return 1;
    char *j = Json_Load(path);
    if (j == NULL) {
        printf("(none)\n");
        return 0;
    }
    size_t len;
    const char *v = Json_FindValue(j, j + strlen(j), "game_version", &len);
    if (v != NULL) {
        printf("%.*s\n", (int)len, v);
    } else {
        printf("(none)\n");
    }
    free(j);
    return 0;
}

/* List directories under .minecraft/versions/ as available versions. */
int Game_Show(void)
{
    char ex[1024];
    char dir[4096];
    plat_dir_t *d;
    const char *name;

    if (Plat_ExeDir(ex, sizeof ex) != 0) return 1;
    if (Plat_Join(dir, sizeof dir, ex, ".minecraft/versions") != 0) return 1;
    d = Plat_Dir_Open(dir);
    if (d == NULL) {
        printf("(no versions directory)\n");
        return 0;
    }
    while (Plat_Dir_Next(d, &name) > 0) {
        if (name[0] == '.') continue;
        char full[8192];
        if (Plat_Join(full, sizeof full, dir, name) != 0) continue;
        if (Plat_IsDir(full)) {
            printf("%s\n", name);
        }
    }
    Plat_Dir_Close(d);
    return 0;
}
