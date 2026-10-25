/*
 * Platform adaptation layer.
 *
 * Every OS-specific behavior (Windows vs POSIX) is confined to this
 * file; business modules only see the unified interfaces in platform.h.
 * Exported functions use the Plat_* prefix; static helpers, when any,
 * are lowercase.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#endif

#include "internal/platform/platform.h"

struct plat_dir {
#ifdef _WIN32
    HANDLE handle;
    WIN32_FIND_DATAA fd;
    int first;
#else
    DIR *dir;
#endif
};

int Plat_ExeDir(char *buf, size_t bufsz)
{
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)bufsz);
    if (n == 0 || n >= bufsz) return -1;
    char *slash = strrchr(buf, '\\');
    if (slash != NULL) *slash = '\0';
    return 0;
#elif defined(__APPLE__)
    uint32_t n = (uint32_t)bufsz;
    if (_NSGetExecutablePath(buf, &n) != 0) return -1;
    char *d = dirname(buf);
    if (d != buf) snprintf(buf, bufsz, "%s", d);
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

int Plat_Join(char *buf, size_t bufsz, const char *base, const char *rel)
{
    int n = snprintf(buf, bufsz, "%s/%s", base, rel);
    if (n < 0 || (size_t)n >= bufsz) {
        return -1;
    }
#ifdef _WIN32
    /* Normalize to native separators, matching GetModuleFileNameA style. */
    for (char *p = buf; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\\';
        }
    }
#endif
    return 0;
}

int Plat_Mkdir(const char *path)
{
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

int Plat_Rmdir(const char *path)
{
#ifdef _WIN32
    return _rmdir(path);
#else
    return rmdir(path);
#endif
}

plat_dir_t *Plat_Dir_Open(const char *path)
{
    plat_dir_t *d = malloc(sizeof *d);
    if (d == NULL) {
        return NULL;
    }
#ifdef _WIN32
    char pattern[4096];
    snprintf(pattern, sizeof pattern, "%s\\*", path);
    d->handle = FindFirstFileA(pattern, &d->fd);
    if (d->handle == INVALID_HANDLE_VALUE) {
        free(d);
        return NULL;
    }
    d->first = 1;
#else
    d->dir = opendir(path);
    if (d->dir == NULL) {
        free(d);
        return NULL;
    }
#endif
    return d;
}

int Plat_Dir_Next(plat_dir_t *d, const char **name)
{
#ifdef _WIN32
    if (!d->first) {
        if (FindNextFileA(d->handle, &d->fd) == 0) {
            return 0;
        }
    }
    d->first = 0;
    *name = d->fd.cFileName;
    return 1;
#else
    struct dirent *e = readdir(d->dir);
    if (e == NULL) {
        return 0;
    }
    *name = e->d_name;
    return 1;
#endif
}

void Plat_Dir_Close(plat_dir_t *d)
{
#ifdef _WIN32
    FindClose(d->handle);
#else
    closedir(d->dir);
#endif
    free(d);
}

int Plat_IsDir(const char *path)
{
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    return attr != INVALID_FILE_ATTRIBUTES
        && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}
