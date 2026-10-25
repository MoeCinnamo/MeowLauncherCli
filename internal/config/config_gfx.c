/* Graphics API config. */
#include <stdio.h>
#include "internal/config/config.h"

int Config_Gfx_Set(const char *api)
{
    char obj[256];
    snprintf(obj, sizeof obj, "{\"gfx_api\":\"%s\"}", api ? api : "auto");
    return config_write_json(obj);
}

int Config_Gfx_Show(void)
{
    char v[64]; config_read_str("gfx_api", "auto", v, sizeof v);
    printf("%s\n", v); return 0;
}
