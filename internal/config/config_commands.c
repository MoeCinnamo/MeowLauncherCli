/* Pre/post command config. */
#include <stdio.h>
#include "internal/config/config.h"

int Config_Precmd_Set(const char *cmd)
{
    char obj[256];
    snprintf(obj, sizeof obj, "{\"pre_cmd\":\"%s\"}", cmd ? cmd : "");
    return config_write_json(obj);
}

int Config_Precmd_Show(void)
{
    char v[256]; config_read_str("pre_cmd", "", v, sizeof v);
    printf("%s\n", v); return 0;
}

int Config_Postcmd_Set(const char *cmd)
{
    char obj[256];
    snprintf(obj, sizeof obj, "{\"post_cmd\":\"%s\"}", cmd ? cmd : "");
    return config_write_json(obj);
}

int Config_Postcmd_Show(void)
{
    char v[256]; config_read_str("post_cmd", "", v, sizeof v);
    printf("%s\n", v); return 0;
}
