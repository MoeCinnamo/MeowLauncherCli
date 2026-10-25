/*
 * account subcommand help. Mirrors internal/help/help.c.
 */

#include <stdio.h>

#include "internal/account/help.h"
#include "internal/i18n/i18n.h"

void Account_Help(void)
{
    printf("%s\n", I18n("account.help.title"));
    printf("  %-32s %s\n", "account local add <user>", I18n("account.help.local_add"));
    printf("  %-32s %s\n", "account local del <user>", I18n("account.help.local_del"));
    printf("  %-32s %s\n", "account local show", I18n("account.help.local_show"));
}
