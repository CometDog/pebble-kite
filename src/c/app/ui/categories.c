#include "categories.h"
#include "../communication.h"
#include "../data.h"
#include "../data_manager.h"
#include "../localization/localization.h"
#include "./navigation.h"
#include "menu_handler.h"
#include <string.h>

static Window *s_main_window;
static DataRegistrationHandle s_registration = NULL;

static char *s_translated_categories[100];
static uint16_t s_translated_count = 0;

static void translate_categories_for_display(void)
{
    const CategoriesData *cat_data = get_categories_data();

    // Clear previous translations
    for (int i = 0; i < s_translated_count; ++i)
    {
        s_translated_categories[i] = NULL;
    }
    s_translated_count = 0;

    // Translate each category name
    for (uint16_t i = 0; i < cat_data->categories_count; ++i)
    {
        const char *translated = localization_translate_category(cat_data->categories[i]);
        s_translated_categories[i] = (char *)translated;
        s_translated_count++;
    }
}

static uint16_t get_categories_count(void)
{
    return get_categories_data()->categories_count;
}

static char **get_categories_list(void)
{
    translate_categories_for_display();

    if (get_categories_data()->categories_count == 0)
    {
        return get_categories_data()->categories;
    }

    return s_translated_categories;
}

static void category_select_handler(int index)
{
    if (!categories_loaded())
    {
        return;
    }

    const CategoriesData *cat_data = get_categories_data();
    if (index < cat_data->categories_count)
    {
        char *category_name = cat_data->categories[index];
        navigation_open_story_titles(category_name);
    }
}

void categories_ui_init(void)
{
    MenuConfig config = {.title = localization_get_string(STRING_CATEGORIES),
                         .get_num_items = get_categories_count,
                         .get_items = get_categories_list,
                         .select_callback = category_select_handler};

    s_main_window = menu_handler_create_window_no_multiline(config);
    DataResource resources[] = {DATA_RESOURCE_CATEGORIES};
    s_registration = data_manager_register_window_requirements(s_main_window, resources, 1);

    window_stack_push(s_main_window, true);
}

void categories_ui_deinit(void)
{
    if (s_registration)
    {
        data_manager_unregister_window_requirements(s_registration);
        s_registration = NULL;
    }
    menu_handler_destroy_window(s_main_window);
}

void request_categories_menu_update(void)
{
    menu_handler_request_update(s_main_window);
}
