#include "industry_impact_detail_list_window.h"
#include "../detail_list_window.h"

static Window *s_detail_list_window = NULL;

void industry_impact_detail_list_window_ui_init(void)
{
    detail_list_window_ui_init_with_default_nav(&s_detail_list_window, DETAIL_TYPE_INDUSTRY_IMPACT);
}

void industry_impact_detail_list_window_ui_deinit(void)
{
    detail_list_window_ui_deinit(&s_detail_list_window);
}

void industry_impact_detail_list_window_ui_update(void)
{
    detail_list_window_ui_update(&s_detail_list_window);
}
