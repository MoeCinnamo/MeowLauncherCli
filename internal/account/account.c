/*
 * Local offline account business logic.
 *
 * Storage and JSON manipulation live in usersfile.c; UUID generation in
 * uuid.c. This file only wires the CLI commands together.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/account/account.h"
#include "internal/account/private.h"
#include "internal/i18n/i18n.h"

int Account_Local_Add(const char *name)
{
    char path[1024];
    char *buf;
    char uuid[37];

    if (name == NULL || name[0] == '\0') {
        fprintf(stderr, "[ERROR] empty username\n");
        return 1;
    }
    if (usersfile_path(path, sizeof path) != 0) {
        fprintf(stderr, "[ERROR] cannot resolve home directory\n");
        return 1;
    }
    buf = usersfile_load(path);
    if (buf == NULL) {
        return 1;
    }
    uuid_generate(uuid, sizeof uuid);
    if (usersfile_insert(&buf, name, uuid) != 0) {
        printf("%s\n", I18n("account.exists"));
        free(buf);
        return 1;
    }
    if (usersfile_save(path, buf) != 0) {
        fprintf(stderr, "[ERROR] cannot write %s\n", path);
        free(buf);
        return 1;
    }
    free(buf);
    printf(I18n("account.added"), name);
    printf("\n");
    return 0;
}

int Account_Local_Del(const char *name)
{
    char path[1024];
    char *buf;

    if (name == NULL || name[0] == '\0') {
        return 1;
    }
    if (usersfile_path(path, sizeof path) != 0) {
        return 1;
    }
    buf = usersfile_load(path);
    if (buf == NULL) {
        return 1;
    }
    if (usersfile_remove(&buf, name) != 0) {
        printf("%s\n", I18n("account.not_found"));
        free(buf);
        return 1;
    }
    usersfile_save(path, buf);
    free(buf);
    printf(I18n("account.removed"), name);
    printf("\n");
    return 0;
}

int Account_Local_Show(void)
{
    char path[1024];
    char *buf;
    if (usersfile_path(path, sizeof path) != 0) return 1;
    buf = usersfile_load(path);
    if (buf == NULL) {
        printf("(none)\n");
        return 0;
    }
    /* Walk the users array and print each name. */
    const char *p = buf;
    while ((p = strstr(p, "\"name\":")) != NULL) {
        p += 7;
        while (*p == ' ' || *p == '\t') p++;
        if (*p != '"') continue;
        p++;
        const char *start = p;
        while (*p && *p != '"') p++;
        printf("%.*s\n", (int)(p - start), start);
    }
    free(buf);
    return 0;
}
