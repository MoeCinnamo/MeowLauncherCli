/*
 * Unit tests for internal/account/help.c.
 */

#include <stdio.h>

#include "internal/account/help.c"

const char *I18n(const char *key) { return key; }

int main(void)
{
    Account_Help();
    return 0;
}
