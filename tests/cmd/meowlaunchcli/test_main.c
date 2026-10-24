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
    const char *bin = getenv("MEOW_BIN");
    char cmd[128];
    if (bin == NULL || bin[0] == '\0') {
        bin = "./build/meowlaunchcli";
    }

    /* Unknown command: exits 1 and prints "Unknown command: <name>". */
    snprintf(cmd, sizeof cmd, "%s thisisbogus 2>&1", bin);
    rc = run_capture(cmd, buf, sizeof buf);
    CHECK(rc == 256, "unknown command exits nonzero");  /* pclose returns status<<8 */
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

    return failures ? 1 : 0;
}
