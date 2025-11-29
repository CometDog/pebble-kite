#include "travel_advisory_detail_text_window.h"
#include "../detail_text_window.h"

static Window *s_detail_window = NULL;

void travel_advisory_detail_text_window_ui_init(void)
{
    detail_text_window_ui_init_with_default_handlers(&s_detail_window, DETAIL_TYPE_TRAVEL_ADVISORY, false);
}

void travel_advisory_detail_text_window_ui_deinit(void)
{
    detail_text_window_ui_deinit(&s_detail_window);
}

void travel_advisory_detail_text_window_ui_update(void)
{
    detail_text_window_ui_update_with_sources_check(&s_detail_window);
}
