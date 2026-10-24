/*
 * Unit tests for internal/help/help.c.
 */

#include "internal/help/help.c"

const char *I18n(const char *key) { return key; }

int main(void)
{
    Help_Print();
    return 0;
}
