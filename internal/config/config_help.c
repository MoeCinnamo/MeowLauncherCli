/* Config help. */
#include <stdio.h>
#include "internal/i18n/i18n.h"
#include "internal/config/config.h"

void Config_Help(void)
{
    printf("%s\n", I18n("config.help.title"));
    printf("  %-30s %s\n", "config java <ver>", I18n("config.help.java_set"));
    printf("  %-30s %s\n", "config java", I18n("config.help.java_show"));
    printf("  %-30s %s\n", "config memory <mb|auto>", I18n("config.help.memory"));
    printf("  %-30s %s\n", "config window <mode> [w h]", I18n("config.help.window"));
    printf("  %-30s %s\n", "config args <jvm_args>", I18n("config.help.args"));
    printf("  %-30s %s\n", "config precmd <cmd>", I18n("config.help.precmd"));
    printf("  %-30s %s\n", "config postcmd <cmd>", I18n("config.help.postcmd"));
    printf("  %-30s %s\n", "config gfx <opengl|vulkan|auto>", I18n("config.help.gfx"));
}
