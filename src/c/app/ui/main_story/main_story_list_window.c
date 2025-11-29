#include "main_story_list_window.h"
#include "../../data.h"
#include "../detail_list_window.h"
#include "../navigation.h"

static Window *s_main_window;

static void on_story_select(int index)
{
    const StoryListData *story_list = get_story_list_data();
    if (story_list->story_titles && story_list->selected_category && index < story_list->story_titles_count)
    {
        navigation_open_story_summary(story_list->selected_category, story_list->story_titles[index]);
    }
}

void main_story_list_window_ui_init()
{
    DataResource resources[] = {DATA_RESOURCE_PRIMARY_STORY_OPTIONS};
    detail_list_window_ui_init(&s_main_window, resources, DETAIL_TYPE_PRIMARY_STORY, on_story_select);
}

void main_story_list_window_ui_deinit(void)
{
    detail_list_window_ui_deinit(&s_main_window);
}

void main_story_list_window_ui_update(void)
{
    detail_list_window_ui_update(&s_main_window);
}