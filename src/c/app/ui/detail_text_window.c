#include "detail_text_window.h"
#include "../data.h"
#include "../detail_types.h"
#include "../utils/debug_logger.h"
#include "detail_text.h"
#include "navigation.h"

static char *get_detail_title_wrapper(void)
{
    return get_detail_text_data()->detail_title;
}

static char *get_detail_text_wrapper(void)
{
    return get_detail_text_data()->detail_text;
}

static void generic_sources_select_handler(void)
{
    const DetailListData *list_data = get_detail_list_data();
    const DetailTextData *text_data = get_detail_text_data();
    if (list_data->detail_type && text_data->detail_title)
    {
        navigation_open_sources(list_data->detail_type, text_data->detail_title);
    }
}

void detail_text_window_ui_init(Window **window, char *(*get_title)(void), char *(*get_text)(void),
                                const char *detail_type, DataResource resource_requirements[],
                                DetailTextSelectCallback on_select)
{
    Window *win = detail_text_create(window, get_title, get_text, detail_type, resource_requirements, 2, NULL);

    if (win && on_select)
    {
        detail_text_set_select_handler(win, on_select);
    }
}

void detail_text_window_ui_deinit(Window **window)
{
    if (window && *window)
    {
        detail_text_destroy(*window);
        *window = NULL;
    }
}

void detail_text_window_ui_update(Window **window)
{
    if (!window || !*window)
        return;
    detail_text_update(*window);
}

void detail_text_window_ui_set_select_handler(Window **window, void (*on_select)(void))
{
    if (!window || !*window)
        return;
    detail_text_set_select_handler(*window, on_select);
}

void detail_text_window_ui_init_with_default_handlers(Window **window, DetailType detail_type, bool set_title_on_init)
{
    const char *api_name = detail_type_get_api_name(detail_type);
    if (!api_name)
    {
        ERROR_LOG("KNCDetailText", "Unknown detail type: %d", detail_type);
        return;
    }

    if (set_title_on_init)
    {
        const char *display_name = detail_type_get_display_name_from_type(detail_type);
        set_detail_title((char *)(display_name ? display_name : api_name), NULL);
    }

    DataResource resources[] = {DATA_RESOURCE_STORY_DETAIL_TITLE, DATA_RESOURCE_STORY_DETAIL_TEXT};

    detail_text_window_ui_init(window, get_detail_title_wrapper, get_detail_text_wrapper, api_name, resources, NULL);
}

void detail_text_window_ui_update_with_sources_check(Window **window)
{
    if (!window || !*window)
        return;

    detail_text_window_ui_update(window);

    if (window_stack_get_top_window() != *window)
        return;

    const DetailSourcesData *sources_data = get_detail_sources_data();
    if (sources_data && sources_data->has_sources)
    {
        detail_text_window_ui_set_select_handler(window, generic_sources_select_handler);
    }
    else
    {
        detail_text_window_ui_set_select_handler(window, NULL);
    }
}
