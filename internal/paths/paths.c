/*
 * Shared executable directory resolver.
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

#include "internal/paths/paths.h"

int ExeDir(char *buf, size_t bufsz)
{
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)bufsz);
    if (n == 0 || n >= bufsz) return -1;
    char *slash = strrchr(buf, '\\');
    if (slash) *slash = '\0';
#elif defined(__APPLE__)
    uint32_t n = (uint32_t)bufsz;
    if (_NSGetExecutablePath(buf, &n) != 0) return -1;
    char *d = dirname(buf);
    if (d != buf) snprintf(buf, bufsz, "%s", d);
#else
    ssize_t n = readlink("/proc/self/exe", buf, bufsz - 1);
    if (n <= 0) return -1;
    buf[n] = '\0';
    char *d = dirname(buf);
    if (d != buf) snprintf(buf, bufsz, "%s", d);
#endif
    return 0;
}
