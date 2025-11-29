#include "background_detail_text_window.h"
#include "../detail_text_window.h"

static Window *s_background_window = NULL;

void background_detail_text_window_ui_init(void)
{
    detail_text_window_ui_init_with_default_handlers(&s_background_window, DETAIL_TYPE_BACKGROUND, true);
}

void background_detail_text_window_ui_deinit(void)
{
    detail_text_window_ui_deinit(&s_background_window);
}

void background_detail_text_window_ui_update(void)
{
    detail_text_window_ui_update_with_sources_check(&s_background_window);
}
