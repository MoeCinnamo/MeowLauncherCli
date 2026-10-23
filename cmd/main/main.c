/*
 * Minecraft CommandMeow Launcher
 *
 * The application logic (initialization, event loop, resource cleanup) is all
 * located in the internal/app module,
 * The entry point is only responsible for calling app_run() and returning its
 * exit code.
 */

#include "internal/app/app.h"

int main(void)
{
    return app_run();
}
