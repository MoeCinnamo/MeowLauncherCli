/* Java version config. */
#include <stdio.h>
#include <string.h>
#include "internal/config/config.h"

int Config_Java_Set(const char *version)
{
    char v[64];
    if (version == NULL || version[0] == '\0') snprintf(v, sizeof v, "auto");
    else snprintf(v, sizeof v, "%s", version);
    char obj[256];
    snprintf(obj, sizeof obj, "{\"java_version\":\"%s\"}", v);
    return config_write_json(obj);
}

int Config_Java_Show(void)
{
    char v[64]; config_read_str("java_version", "auto", v, sizeof v);
    printf("%s\n", v); return 0;
}
