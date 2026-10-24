#include <stdio.h>
#include "internal/game/help.h"
#include "internal/i18n/i18n.h"

void Game_Help(void)
{
    printf("%s\n", I18n("game.help.title"));
    printf("  %-32s %s\n", "game switch <ver>", I18n("game.help.switch"));
    printf("  %-32s %s\n", "game current", I18n("game.help.current"));
    printf("  %-32s %s\n", "game show", I18n("game.help.show"));
}
