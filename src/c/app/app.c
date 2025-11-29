#include "app.h"
#include "communication.h"
#include "data.h"
#include "data_manager.h"
#include "ui/categories.h"
#include "ui/splash_screen.h"

static void init(void)
{
    communication_init();
    splash_screen_init();
    data_init();
    data_manager_init();
}

static void deinit(void)
{
    if (splash_screen_is_showing())
    {
        splash_screen_deinit();
    }
    else
    {
        categories_ui_deinit();
    }
    communication_deinit();
    data_deinit();
    data_manager_deinit();
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
    return 0;
}
