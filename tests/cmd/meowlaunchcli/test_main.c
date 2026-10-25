/*
 * Entry-point tests for cmd/meowlaunchcli/main.c.
 *
 * The entry point is a real binary; these tests run it as a subprocess and
 * assert on exit codes and stderr/stdout. Built by scripts/test.sh.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond, name)                                              \
    do {                                                               \
        if (cond) { printf("[PASS] %s\n", name); }                     \
        else { printf("[FAIL] %s\n", name); failures++; }              \
    } while (0)

/* Run a command, capture combined output into buf, return exit status. */
static int run_capture(const char *cmd, char *buf, size_t bufsz)
{
    FILE *p = popen(cmd, "r");
    if (p == NULL) {
        buf[0] = '\0';
        return -1;
    }
    size_t n = fread(buf, 1, bufsz - 1, p);
    buf[n] = '\0';
    return pclose(p);
}

int main(void)
{
    char buf[512];
    int rc;
    const char *bin = getenv("MEOWLAUNCH_BIN");
    char cmd[128];
    if (bin == NULL || bin[0] == '\0') {
        bin = "./build/meowlaunchcli";
    }

    /* Unknown command: exits nonzero and prints "Unknown command: <name>".
     * pclose status encoding differs across platforms; just require nonzero. */
    snprintf(cmd, sizeof cmd, "%s thisisbogus 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc != 0, "unknown command exits nonzero");
    CHECK(strstr(buf, "Unknown command: thisisbogus") != NULL,
          "unknown command message names the bad command");

    /* No arguments: exits 0 and prints the usage line. */
    snprintf(cmd, sizeof cmd, "%s 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "bare invocation exits 0");
    CHECK(strstr(buf, "Usage: meowlaunchcli") != NULL,
          "bare invocation prints usage");

    /* version: exits 0. */
    snprintf(cmd, sizeof cmd, "%s version 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "version exits 0");
    CHECK(strstr(buf, "MeowLauncherCli v") != NULL,
          "version prints banner");

    /* account help: exits 0 and prints account help. */
    snprintf(cmd, sizeof cmd, "%s account help 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "account help exits 0");
    CHECK(strstr(buf, "Account management") != NULL,
          "account help prints title");

    /* account with bad subcommand: exits nonzero, prints usage. */
    snprintf(cmd, sizeof cmd, "%s account bogus 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc != 0, "account bogus exits nonzero");
    CHECK(strstr(buf, "account local") != NULL,
          "account bogus prints usage");

    /* account local add/del round-trip. */
    snprintf(cmd, sizeof cmd, "%s account local add cmdtester 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "account local add exits 0");
    snprintf(cmd, sizeof cmd, "%s account local del cmdtester 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "account local del exits 0");

    /* account local show: exits 0. */
    snprintf(cmd, sizeof cmd, "%s account local show 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "account local show exits 0");

    /* game help: exits 0. */
    snprintf(cmd, sizeof cmd, "%s game help 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "game help exits 0");

    /* game with no subcommand: exits nonzero, prints usage. */
    snprintf(cmd, sizeof cmd, "%s game 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc != 0, "game no subcommand exits nonzero");
    CHECK(strstr(buf, "Usage: meowlaunchcli game") != NULL,
          "game no subcommand prints usage");

    /* game switch/current/show. */
    snprintf(cmd, sizeof cmd, "%s game switch 1.21.11 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "game switch exits 0");
    snprintf(cmd, sizeof cmd, "%s game current 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "game current exits 0");
    snprintf(cmd, sizeof cmd, "%s game show 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "game show exits 0");

    /* game switch with no version: falls back to Game_Current(). */
    snprintf(cmd, sizeof cmd, "%s game switch 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "game switch bare exits 0");

    /* config help: exits 0. */
    snprintf(cmd, sizeof cmd, "%s config help 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config help exits 0");

    /* config with unknown subcommand: prints usage, exits nonzero. */
    snprintf(cmd, sizeof cmd, "%s config bogus 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc != 0, "config bogus exits nonzero");
    CHECK(strstr(buf, "config java") != NULL,
          "config bogus prints usage");

    /* config java set/show. */
    snprintf(cmd, sizeof cmd, "%s config java 21 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config java set exits 0");
    snprintf(cmd, sizeof cmd, "%s config java 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config java show exits 0");

    /* config memory/window/args/precmd/postcmd/gfx. */
    snprintf(cmd, sizeof cmd, "%s config memory 2048 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config memory set exits 0");
    snprintf(cmd, sizeof cmd, "%s config window windowed 1280 720 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config window set exits 0");
    snprintf(cmd, sizeof cmd, "%s config args -Xmx2G 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config args set exits 0");
    snprintf(cmd, sizeof cmd, "%s config precmd option 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config precmd set exits 0");
    snprintf(cmd, sizeof cmd, "%s config postcmd done 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config postcmd set exits 0");
    snprintf(cmd, sizeof cmd, "%s config gfx opengl 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config gfx set exits 0");

    /* config bare-arg variants: defaults, empty values, show paths. */
    snprintf(cmd, sizeof cmd, "%s config memory 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config memory bare exits 0");
    snprintf(cmd, sizeof cmd, "%s config memory auto 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config memory auto exits 0");
    snprintf(cmd, sizeof cmd, "%s config window windowed 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config window defaults exits 0");
    snprintf(cmd, sizeof cmd, "%s config args 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config args bare exits 0");
    snprintf(cmd, sizeof cmd, "%s config precmd 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config precmd bare exits 0");
    snprintf(cmd, sizeof cmd, "%s config postcmd 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config postcmd bare exits 0");
    snprintf(cmd, sizeof cmd, "%s config gfx 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 0, "config gfx show exits 0");

    return failures ? 1 : 0;
}
