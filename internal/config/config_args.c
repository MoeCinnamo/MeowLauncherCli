/* JVM args config. */
#include <stdio.h>
#include "internal/config/config.h"

int Config_Java_Args_Set(const char *args)
{
    char obj[256];
    snprintf(obj, sizeof obj, "{\"jvm_args\":\"%s\"}", args ? args : "");
    return config_write_json(obj);
}

int Config_Java_Args_Show(void)
{
    char v[256]; config_read_str("jvm_args", "", v, sizeof v);
    printf("%s\n", v); return 0;
}
