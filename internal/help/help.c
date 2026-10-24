/*
 * Help text for MeowLauncherCli.
 *
 * All user-facing strings are pulled from the embedded language archives
 * via I18n(); nothing translatable lives here.
 */

#include <stdio.h>

#include "internal/help/help.h"
#include "internal/i18n/i18n.h"

void Help_Print(void)
{
    printf("MeowLauncherCli — %s\n\n", I18n("help.tagline"));
    printf("%s\n\n", I18n("help.usage"));
    printf("%s\n", I18n("help.commands_header"));
    printf("  %-10s %s\n", "help",     I18n("help.cmd_help"));
    printf("  %-10s %s\n", "version",  I18n("help.cmd_version"));
    printf("  %-10s %s\n", "install",  I18n("help.cmd_install"));
    printf("  %-10s %s\n", "remove",   I18n("help.cmd_remove"));
    printf("  %-10s %s\n", "update",   I18n("help.cmd_update"));
    printf("  %-10s %s\n", "search",   I18n("help.cmd_search"));
    printf("  %-10s %s\n", "game",     I18n("help.cmd_game"));
    printf("  %-10s %s\n", "launch",   I18n("help.cmd_launch"));
    printf("  %-10s %s\n", "mod",      I18n("help.cmd_mod"));
    printf("  %-10s %s\n", "account",  I18n("help.cmd_account"));
    printf("  %-10s %s\n", "world",    I18n("help.cmd_world"));
    printf("  %-10s %s\n", "resource", I18n("help.cmd_resource"));
    printf("  %-10s %s\n", "config",   I18n("help.cmd_config"));
    printf("\n%s\n", I18n("help.see_help"));
}
