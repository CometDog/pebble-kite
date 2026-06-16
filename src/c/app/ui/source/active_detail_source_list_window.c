#include "active_detail_source_list_window.h"
#include "../../data.h"
#include "../../localization/localization.h"
#include "../detail_list_window.h"
#include "../navigation.h"
#include <string.h>

static Window *s_main_window;

static void on_active_source_select(int index)
{
    const DetailSourcesData *sources_data = get_detail_sources_data();
    if (sources_data->sources && index < sources_data->sources_count)
    {
        const char *source = sources_data->sources[index];
        if (source && strcmp(source, localization_get_string(STRING_LOADING)) != 0)
        {
            // For Quote detail type, don't overwrite the title (which is the author name)
            const DetailListData *list_data = get_detail_list_data();
            if (!list_data->detail_type || strcmp(list_data->detail_type, "Quote") != 0)
            {
                set_detail_title((char *)source, NULL);
            }

            if (list_data && list_data->detail_type && strcmp(list_data->detail_type, "Quote") == 0)
            {
                navigation_open_qr_code("Type_Quote");
            }
            else
            {
                navigation_open_qr_code(source);
            }
        }
    }
}

void active_detail_source_list_window_ui_init(void)
{
    DataResource resources[] = {DATA_RESOURCE_ACTIVE_DETAIL_SOURCE_OPTIONS};
    detail_list_window_ui_init(&s_main_window, resources, DETAIL_TYPE_DETAIL_SOURCES, on_active_source_select);
}

void active_detail_source_list_window_ui_deinit(void)
{
    detail_list_window_ui_deinit(&s_main_window);
}

void active_detail_source_list_window_ui_update(void)
{
    detail_list_window_ui_update(&s_main_window);
}
