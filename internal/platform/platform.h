#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>

/* Opaque directory iterator. */
typedef struct plat_dir plat_dir_t;

/* Write the directory of the current executable into buf, without a
 * trailing separator. Returns 0 on success, nonzero on error. */
int Plat_ExeDir(char *buf, size_t bufsz);

/* Join base, the platform separator and rel into buf. Forward slashes
 * in rel are normalized to the platform separator ('/' on POSIX, '\\'
 * on Windows). Returns 0 on success, nonzero when buf is too small. */
int Plat_Join(char *buf, size_t bufsz, const char *base, const char *rel);

/* Create a single directory. Returns 0 on success, nonzero on error. */
int Plat_Mkdir(const char *path);

/* Remove an empty directory. Returns 0 on success, nonzero on error. */
int Plat_Rmdir(const char *path);

/* Open a directory for iteration, including dot entries. Returns NULL
 * on error. */
plat_dir_t *Plat_Dir_Open(const char *path);

/* Fetch the next entry name. Returns 1 with *name set, 0 at the end of
 * the directory, -1 on error. *name stays valid until the next call or
 * Plat_Dir_Close. */
int Plat_Dir_Next(plat_dir_t *dir, const char **name);

/* Close an iterator opened by Plat_Dir_Open. */
void Plat_Dir_Close(plat_dir_t *dir);

/* True (1) when path is an existing directory, 0 otherwise. */
int Plat_IsDir(const char *path);

#endif
