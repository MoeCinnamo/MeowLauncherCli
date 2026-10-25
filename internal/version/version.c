/*
 * Version information for MeowLauncherCli.
 *
 * Single source of truth for the launcher's own version string. Bump
 * MEOW_VERSION on every release. The git build identifier is injected at
 * compile time by the build system, for example:
 *   -DGIT_VERSION="$(git describe --always --dirty)"
 * and falls back to "unknown" when the build does not define it.
 */

#include <stdio.h>

#include "internal/version/version.h"

/* Launcher semantic version; bump on each release. */
#define MEOW_VERSION "0.0.1"

#ifndef GIT_VERSION
#define GIT_VERSION "unknown"
#endif

void Version_Print(void)
{
    fprintf(stdout, "MeowLauncherCli v%s build %s\n",
            MEOW_VERSION, GIT_VERSION);
}
