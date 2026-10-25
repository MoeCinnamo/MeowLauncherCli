/*
 * UUID v4-style generator: 8-4-4-4-12 hex groups.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "internal/account/private.h"

void uuid_generate(char *out, size_t outsz)
{
    static const char hex[] = "0123456789abcdef";
    static const int groups[5] = {8, 4, 4, 4, 12};
    int g, i, pos = 0;
    static int seeded = 0;
    (void)outsz;

    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
    for (g = 0; g < 5; g++) {
        for (i = 0; i < groups[g]; i++) {
            out[pos++] = hex[rand() & 0xf];
        }
        if (g < 4) {
            out[pos++] = '-';
        }
    }
    out[pos] = '\0';
}
