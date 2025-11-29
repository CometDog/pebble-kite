#include "main_story_text_window.h"
#include "../../communication.h"
#include "../../data.h"
#include "../detail_text_window.h"
#include "../navigation.h"

static Window *s_main_window;

static char *get_story_title_wrapper(void)
{
    return get_story_data()->story_full_title;
}

static char *get_story_summary_wrapper(void)
{
    return get_story_data()->story_short_summary;
}

static void on_select_handler(void)
{
    const StoryListData *list_data = get_story_list_data();
    const StoryData *story_data = get_story_data();
    navigation_open_available_details(list_data->selected_category, story_data->story_id);
}

void main_story_text_window_ui_init(void)
{
    DataResource resources[] = {DATA_RESOURCE_PRIMARY_STORY_TITLE, DATA_RESOURCE_PRIMARY_STORY_TEXT};
    detail_text_window_ui_init(&s_main_window, get_story_title_wrapper, get_story_summary_wrapper, NULL, resources,
                               on_select_handler);
}

void main_story_text_window_ui_deinit(void)
{
    detail_text_window_ui_deinit(&s_main_window);
}

void main_story_text_window_ui_update(void)
{
    detail_text_window_ui_update(&s_main_window);
}
