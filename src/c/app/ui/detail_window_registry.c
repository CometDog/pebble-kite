#include "detail_window_registry.h"
#include "../detail_types.h"
#include "detail_list_window.h"
#include "detail_text_window.h"

static Window *s_current_list_window = NULL;
static Window *s_current_text_window = NULL;
static DetailType s_current_list_type = (DetailType)-1;
static DetailType s_current_text_type = (DetailType)-1;

void detail_window_registry_init_list(DetailType detail_type)
{
    if (s_current_list_window)
    {
        detail_list_window_ui_deinit(&s_current_list_window);
        s_current_list_window = NULL;
    }

    detail_list_window_ui_init_with_default_nav(&s_current_list_window, detail_type);
    s_current_list_type = detail_type;
}

void detail_window_registry_init_text(DetailType detail_type)
{
    if (s_current_text_window)
    {
        detail_text_window_ui_deinit(&s_current_text_window);
        s_current_text_window = NULL;
    }

    const DetailTypeInfo *info = detail_type_from_type(detail_type);
    bool set_title_on_init = (info && !info->has_list);

    detail_text_window_ui_init_with_default_handlers(&s_current_text_window, detail_type, set_title_on_init);
    s_current_text_type = detail_type;
}

void detail_window_registry_deinit_list(void)
{
    if (s_current_list_window)
    {
        detail_list_window_ui_deinit(&s_current_list_window);
        s_current_list_window = NULL;
        s_current_list_type = (DetailType)-1;
    }
}

void detail_window_registry_deinit_text(void)
{
    if (s_current_text_window)
    {
        detail_text_window_ui_deinit(&s_current_text_window);
        s_current_text_window = NULL;
        s_current_text_type = (DetailType)-1;
    }
}

void detail_window_registry_update_list(void)
{
    if (s_current_list_window)
    {
        detail_list_window_ui_update(&s_current_list_window);
    }
}

void detail_window_registry_update_text(void)
{
    if (s_current_text_window)
    {
        detail_text_window_ui_update_with_sources_check(&s_current_text_window);
    }
}
