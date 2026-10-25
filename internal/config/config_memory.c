/* Memory config. */
#include <stdio.h>
#include "internal/config/config.h"

int Config_Memory_Set(int mb)
{
    char obj[256];
    if (mb <= 0) snprintf(obj, sizeof obj, "{\"memory\":\"auto\"}");
    else snprintf(obj, sizeof obj, "{\"memory_mb\":%d}", mb);
    return config_write_json(obj);
}

int Config_Memory_Show(void)
{
    char v[64]; config_read_str("memory", "auto", v, sizeof v);
    printf("%s\n", v); return 0;
}
