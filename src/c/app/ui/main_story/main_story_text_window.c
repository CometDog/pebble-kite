#include "main_story_text_window.h"
#include "../../communication.h"
#include "../../data.h"
#include "../detail_text.h"
#include "../detail_text_window.h"
#include "../navigation.h"
#include <string.h>

static Window *s_main_window;
static bool s_marked_as_read = false;
static int s_story_index = 0;

static char *get_story_title_wrapper(void)
{
    return get_story_data()->story_full_title;
}

static char *get_story_summary_wrapper(void)
{
    return get_story_data()->story_short_summary;
}

static void mark_story_as_read(void)
{
    if (s_marked_as_read)
        return;

    const StoryData *story_data = get_story_data();

    send_mark_story_read(story_data->story_id); // To phone
    set_story_read(s_story_index, true);        // Local data
    s_marked_as_read = true;
}

static void on_scroll_handler(ScrollLayer *scroll_layer, void *context)
{
    mark_story_as_read();
}

static void on_select_handler(void)
{
    mark_story_as_read();
    const StoryListData *list_data = get_story_list_data();
    const StoryData *story_data = get_story_data();
    if (strncmp(list_data->selected_category, "TensionIndex", 12) == 0)
    {
        navigation_open_detail_text("TensionIndex", story_data->story_short_summary);
    }
    else
    {
        navigation_open_available_details(list_data->selected_category, story_data->story_id);
    }
}

void main_story_text_window_ui_init(int index)
{
    s_story_index = index;
    s_marked_as_read = false;
    DataResource resources[] = {DATA_RESOURCE_PRIMARY_STORY_TITLE, DATA_RESOURCE_PRIMARY_STORY_TEXT};

    detail_text_window_ui_init(&s_main_window, get_story_title_wrapper, get_story_summary_wrapper, NULL, resources,
                               on_select_handler);

    if (s_main_window)
    {
        detail_text_set_scroll_handler(s_main_window, on_scroll_handler);
    }
}

void main_story_text_window_ui_deinit(void)
{
    detail_text_window_ui_deinit(&s_main_window);
}

void main_story_text_window_ui_update(void)
{
    detail_text_window_ui_update(&s_main_window);
}
