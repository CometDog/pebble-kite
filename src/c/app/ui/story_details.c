#include "story_details.h"
#include "../communication.h"
#include "../data.h"
#include "../data_manager.h"
#include "../detail_types.h"
#include "../localization/localization.h"
#include "menu_handler.h"
#include "navigation.h"
#include <stdlib.h>

static Window *s_main_window;
static DataRegistrationHandle s_registration = NULL;

static uint16_t get_details_count(void)
{
    return get_available_details_data()->available_details_count;
}

static void detail_select_handler(int index)
{
    const AvailableDetailsData *details_data = get_available_details_data();
    if (index >= 0 && index < details_data->available_details_count)
    {
        const char *selected_detail = details_data->available_details[index];
        // Use navigation system to route to appropriate tier
        navigation_open_detail_content(selected_detail);
    }
}

char **get_modified_item_names(void)
{
    const AvailableDetailsData *details_data = get_available_details_data();
    char **details = details_data->available_details;
    size_t count = details_data->available_details_count;

    static char *modified_details[15];
    size_t max_count = 15;
    for (uint16_t i = 0; i < count && i < max_count; i++)
    {
        modified_details[i] = (char *)detail_type_get_display_name(details[i]);
    }

    // Null out the rest
    for (uint16_t j = count; j < max_count; ++j)
    {
        modified_details[j] = NULL;
    }
    return modified_details;
}

static void story_details_window_appear(Window *window)
{
    menu_handler_window_appeared(window);
}

static void story_details_window_disappear(Window *window)
{
    menu_handler_window_disappeared(window);
}

static void story_details_window_unload(Window *window)
{
    story_details_ui_deinit();
}

void story_details_ui_init(void)
{
    MenuConfig config = {.title = localization_get_string(STRING_AVAILABLE_DETAILS),
                         .get_num_items = get_details_count,
                         .get_items = get_modified_item_names,
                         .select_callback = detail_select_handler};

    s_main_window = menu_handler_create_window(config);
    if (s_main_window)
    {
        window_set_window_handlers(s_main_window, (WindowHandlers){.appear = story_details_window_appear,
                                                                   .disappear = story_details_window_disappear,
                                                                   .unload = story_details_window_unload});
    }
    DataResource resources[] = {DATA_RESOURCE_STORY_DETAILS};
    s_registration = data_manager_register_window_requirements(s_main_window, resources, 1);
    window_stack_push(s_main_window, true);
}

void story_details_ui_deinit(void)
{
    clear_available_details_data();
    if (s_main_window)
    {
        if (s_registration)
        {
            data_manager_unregister_window_requirements(s_registration);
            s_registration = NULL;
        }
        menu_handler_destroy_window(s_main_window);
        s_main_window = NULL;
    }
}

void request_available_details_menu_update(void)
{
    menu_handler_request_update(s_main_window);
}
