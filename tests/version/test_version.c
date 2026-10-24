/*
 * Unit tests for internal/version.
 *
 * Built by scripts/test.sh together with internal/version/version.c.
 */

#include <stdio.h>

#include "internal/version/version.h"

int main(void)
{
    /* Smoke: printing must not crash (output goes to stdout). */
    Version_Print();
    printf("[PASS] version.print runs cleanly\n");
    return 0;
}
