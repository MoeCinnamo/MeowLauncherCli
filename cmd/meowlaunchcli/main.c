/*
 * MeowLauncherCli entry point.
 *
 * Non-interactive, busybox-style: running without arguments (or with
 * "help") prints the help list; otherwise the first argument selects a
 * subcommand. There is no interactive prompt loop.
 */

#include <stdio.h>
#include <string.h>

#include "internal/account/account.h"
#include "internal/help/help.h"
#include "internal/i18n/i18n.h"
#include "internal/version/version.h"

int main(int argc, char **argv)
{
    I18n_Init();

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        Help_Print();
        return 0;
    }

    if (strcmp(argv[1], "version") == 0) {
        Version_Print();
        return 0;
    }

    if (strcmp(argv[1], "account") == 0) {
        if (argc >= 5 && strcmp(argv[2], "local") == 0) {
            if (strcmp(argv[3], "add") == 0) {
                return Account_Local_Add(argv[4]);
            }
            if (strcmp(argv[3], "del") == 0) {
                return Account_Local_Del(argv[4]);
            }
        }
        fprintf(stderr, "%s\n", I18n("account.usage"));
        return 1;
    }

    /* TODO: subcommand dispatch table goes here. */
    fprintf(stderr, I18n("error.unknown_command"), argv[1]);
    fprintf(stderr, "\n");
    return 1;
}
