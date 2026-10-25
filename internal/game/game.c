/*
 * Current game version stored in <exedir>/.meowlaunch/config.json.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/json/json.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

static int exedir(char *buf, size_t bufsz)
{
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)bufsz);
    if (n == 0 || n >= bufsz) return -1;
    char *slash = strrchr(buf, '\\');
    if (slash) *slash = '\0';
    return 0;
#else
    ssize_t n = readlink("/proc/self/exe", buf, bufsz - 1);
    if (n <= 0) return -1;
    buf[n] = '\0';
    char *d = dirname(buf);
    if (d != buf) snprintf(buf, bufsz, "%s", d);
    return 0;
#endif
}

static int config_path(char *buf, size_t bufsz)
{
    char dir[1024];
    if (exedir(dir, sizeof dir) != 0) return -1;
#ifdef _WIN32
    return snprintf(buf, bufsz, "%s\\.meowlaunch\\config.json", dir) < 0;
#else
    return snprintf(buf, bufsz, "%s/.meowlaunch/config.json", dir) < 0;
#endif
}

/* Set the current game version. Returns 0 on success. */
int Game_Switch(const char *version)
{
    char path[1024];
    char buf[256];
    snprintf(buf, sizeof buf, "{\"game_version\":\"%s\"}", version);
    if (config_path(path, sizeof path) != 0) return 1;
    return Json_Save(path, buf) != 0;
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
    if (exedir(ex, sizeof ex) != 0) return 1;
#ifdef _WIN32
    char pattern[4096];
    snprintf(pattern, sizeof pattern, "%s\\.minecraft\\versions\\*", ex);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        printf("(no versions directory)\n");
        return 0;
    }
    do {
        if (fd.cFileName[0] == '.') continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("%s\n", fd.cFileName);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    char dir[4096];
    snprintf(dir, sizeof dir, "%s/.minecraft/versions", ex);
    DIR *d = opendir(dir);
    if (d == NULL) {
        printf("(no versions directory)\n");
        return 0;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.') continue;
        char full[8192];
        snprintf(full, sizeof full, "%s/%s", dir, e->d_name);
        struct stat st;
        if (stat(full, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("%s\n", e->d_name);
        }
    }
    closedir(d);
#endif
    return 0;
}
