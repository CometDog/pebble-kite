#include "international_reactions_detail_list_window.h"
#include "../detail_list_window.h"

static Window *s_main_window;

void international_reactions_detail_list_window_ui_init(void)
{
    detail_list_window_ui_init_with_default_nav(&s_main_window, DETAIL_TYPE_INTERNATIONAL_REACTIONS);
}

void international_reactions_detail_list_window_ui_deinit(void)
{
    detail_list_window_ui_deinit(&s_main_window);
}

void international_reactions_detail_list_window_ui_update(void)
{
    detail_list_window_ui_update(&s_main_window);
}
