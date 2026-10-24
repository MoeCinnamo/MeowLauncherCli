#ifndef MEOW_HELP_HELP_H
#define MEOW_HELP_HELP_H

/*
 * Print the top-level help (usage and command list) to stdout.
 *
 * Called when the launcher runs without arguments, or with the "help"
 * subcommand. Busybox-style: lists every available command group.
 */
void Help_Print(void);

#endif /* MEOW_HELP_HELP_H */
