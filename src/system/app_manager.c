// app_manager.c
#include "app_manager.h"
#include "../launcher/launcher.h"

void app_manager_start(void)
{
    launcher_open();  // 🔁 can be replaced later with resume logic or last open app
}
