#include "internal/config/config_common.c"
#include "internal/config/config_java.c"
#include "internal/config/config_memory.c"
#include "internal/config/config_window.c"
#include "internal/config/config_args.c"
#include "internal/config/config_commands.c"
#include "internal/config/config_gfx.c"
#include "internal/config/config_help.c"
#include "internal/paths/paths.c"
#include "internal/json/json_io.c"
#include "internal/json/json_walk.c"

const char *I18n(const char *key) { return key; }

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("[PASS] %s\n", name); \
    else { printf("[FAIL] %s\n", name); failures++; } \
} while (0)

int main(void)
{
    CHECK(Config_Java_Set("21") == 0, "java set");
    CHECK(Config_Java_Show() == 0, "java show");
    CHECK(Config_Memory_Set(2048) == 0, "memory set");
    CHECK(Config_Memory_Show() == 0, "memory show");
    CHECK(Config_Window_Set("windowed", 1280, 720) == 0, "window set");
    CHECK(Config_Window_Show() == 0, "window show");
    CHECK(Config_Java_Args_Set("-Xmx2G") == 0, "args set");
    CHECK(Config_Java_Args_Show() == 0, "args show");
    CHECK(Config_Precmd_Set("option") == 0, "precmd set");
    CHECK(Config_Precmd_Show() == 0, "precmd show");
    CHECK(Config_Postcmd_Set("echo done") == 0, "postcmd set");
    CHECK(Config_Postcmd_Show() == 0, "postcmd show");
    CHECK(Config_Gfx_Set("opengl") == 0, "gfx set");
    CHECK(Config_Gfx_Show() == 0, "gfx show");
    Config_Help();
    return failures ? 1 : 0;
}
