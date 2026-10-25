/* Window config. */
#include <stdio.h>
#include "internal/config/config.h"

int Config_Window_Set(const char *mode, int w, int h)
{
    char obj[256];
    snprintf(obj, sizeof obj, "{\"window_mode\":\"%s\",\"width\":%d,\"height\":%d}",
             mode ? mode : "windowed", w > 0 ? w : 1280, h > 0 ? h : 720);
    return config_write_json(obj);
}

int Config_Window_Show(void)
{
    char v[64]; config_read_str("window_mode", "windowed", v, sizeof v);
    printf("%s\n", v); return 0;
}
