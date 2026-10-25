/*
 * JSON file IO: Json_Load / Json_Save.
 */

#include <stdio.h>
#include <stdlib.h>

#include "internal/json/json.h"
#include "internal/platform/platform.h"

char *Json_Load(const char *path)
{
    FILE *f = fopen(path, "rb");
    long n;
    char *buf;
    if (f == NULL) {
        return NULL;
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

int Json_Save(const char *path, const char *buf)
{
    char tmp[1024];
    FILE *out;
    char *p;

    snprintf(tmp, sizeof tmp, "%s", path);
    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = '\0';
            Plat_Mkdir(tmp); /* may already exist; failure is expected */
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
