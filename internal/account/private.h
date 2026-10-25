#ifndef ACCOUNT_PRIVATE_H
#define ACCOUNT_PRIVATE_H

/* Internal cross-TU declarations for the account module. Not for use
 * outside internal/account/. */

void uuid_generate(char *out, size_t outsz);

int usersfile_path(char *buf, size_t bufsz);
char *usersfile_load(const char *path);
int usersfile_save(const char *path, const char *buf);
int usersfile_insert(char **buf, const char *name, const char *uuid);
int usersfile_remove(char **buf, const char *name);

#endif
