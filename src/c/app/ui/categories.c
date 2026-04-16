// TODO: I should drop single section support entirely in the future, just making multi section fall back to single
// section mode naturally. This will simplify this file to get rid of the now redundant single-section callbacks.

#include "categories.h"
#include "../communication.h"
#include "../data.h"
#include "../data_manager.h"
#include "../localization/localization.h"
#include "./navigation.h"
#include "menu_handler.h"
#include "ui_config.h"
#include <string.h>

static Window *s_main_window;
static TextLayer *status_bar_background_layer;
static TextLayer *status_bar_text_layer;
static DataRegistrationHandle s_registration = NULL;

// TODO: This value for the array should be based on the MAX_ITEMS for categories in the data file
static char *s_translated_categories[20];
static uint16_t s_translated_count = 0;

static void translate_categories_for_display(void)
{
    const CategoriesData *category_data = get_categories_data();

    // Clear previous translations
    for (int i = 0; i < s_translated_count; ++i)
    {
        s_translated_categories[i] = NULL;
    }
    s_translated_count = 0;

    // Translate category given the index
    for (uint16_t i = 0; i < category_data->categories_count && i < 20; ++i)
    {
        const char *translated = localization_get_category_by_index(i);
        if (translated && translated[0] != '\0')
        {
            s_translated_categories[i] = (char *)translated;
        }
        else
        {
            s_translated_categories[i] = category_data->categories[i];
        }
        s_translated_count++;
    }
}

static uint16_t get_num_sections(void)
{
    uint8_t additional_feeds_count = get_additional_feeds_count();
    return (additional_feeds_count > 0) ? 2 : 1;
}

static uint16_t get_num_items_in_section(uint16_t section_index)
{
    uint8_t additional_feeds_count = get_additional_feeds_count();

    if (additional_feeds_count == 0)
    {
        return get_categories_data()->categories_count;
    }

    if (section_index == 0)
    {
        return additional_feeds_count;
    }
    else
    {
        const CategoriesData *category_data = get_categories_data();
        return category_data->categories_count - additional_feeds_count;
    }
}

static char **get_items_in_section(uint16_t section_index)
{
    translate_categories_for_display();

    uint8_t additional_feeds_count = get_additional_feeds_count();

    if (additional_feeds_count == 0)
    {
        return s_translated_categories;
    }

    if (section_index == 0)
    {
        return s_translated_categories;
    }
    else
    {
        return &s_translated_categories[additional_feeds_count];
    }
}

static const char *get_section_title(uint16_t section_index)
{
    uint8_t additional_feeds_count = get_additional_feeds_count();

    if (additional_feeds_count == 0)
    {
        return localization_get_string(STRING_CATEGORIES);
    }

    if (section_index == 0)
    {
        return localization_get_string(STRING_FEEDS);
    }
    else
    {
        return localization_get_string(STRING_NEWS);
    }
}

static void up_press_handler_from_top_of_list(void)
{
    navigation_open_story_summary("TensionIndex", "", 0);
}

static void category_select_handler_in_section(uint16_t section_index, uint16_t row_index)
{
    if (!categories_loaded())
    {
        return;
    }

    const CategoriesData *cat_data = get_categories_data();
    uint8_t additional_feeds_count = get_additional_feeds_count();

    uint16_t actual_index;
    if (additional_feeds_count > 0 && section_index == 1)
    {
        actual_index = additional_feeds_count + row_index;
    }
    else
    {
        actual_index = row_index;
    }

    if (actual_index < cat_data->categories_count)
    {
        char *category_name = cat_data->categories[actual_index];
        navigation_open_story_titles(category_name, actual_index);
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
        navigation_open_story_titles(category_name, index);
    }
}

static void init_tension_status_bar_for_window(Window *window)
{
    Layer *root_layer = window_get_root_layer(window);
    GRect status_bar_layer_background_frame =
        GRect(0, 0, layer_get_bounds(root_layer).size.w, UI_CUSTOM_STATUS_BAR_HEIGHT);
    status_bar_background_layer = text_layer_create(status_bar_layer_background_frame);
    text_layer_set_background_color(status_bar_background_layer, GColorBlack);
    layer_add_child(root_layer, text_layer_get_layer(status_bar_background_layer));

    GRect status_bar_layer_frame = GRect(0, UI_CUSTOM_STATUS_BAR_HEIGHT - UI_LINE_HEIGHT_REGULAR,
                                         layer_get_bounds(root_layer).size.w, UI_LINE_HEIGHT_REGULAR);
    status_bar_text_layer = text_layer_create(status_bar_layer_frame);
    text_layer_set_background_color(status_bar_text_layer, GColorBlack);
    text_layer_set_text_color(status_bar_text_layer, GColorWhite);
    text_layer_set_text_alignment(status_bar_text_layer, GTextAlignmentCenter);
    layer_add_child(root_layer, text_layer_get_layer(status_bar_text_layer));
    text_layer_set_font(status_bar_text_layer, ui_get_system_font_status_bar());
}

void categories_ui_init(void)
{
    bool isTense = get_tension_index_data()->tension_index > 0;
    MenuConfig config = {.title = localization_get_string(STRING_CATEGORIES),
                         .has_status_bar = isTense,
                         .get_num_sections = get_num_sections,
                         .get_num_items = get_categories_count,
                         .get_num_items_in_section = get_num_items_in_section,
                         .get_items = get_categories_list,
                         .get_items_in_section = get_items_in_section,
                         .get_section_title = get_section_title,
                         .select_callback = category_select_handler,
                         .select_callback_in_section = category_select_handler_in_section,
                         .up_press_handler_from_top_of_list = isTense ? up_press_handler_from_top_of_list : NULL};
    s_main_window = menu_handler_create_window_no_multiline(config);
    DataResource resources[] = {DATA_RESOURCE_CATEGORIES};
    s_registration = data_manager_register_window_requirements(s_main_window, resources, 1);

    if (isTense)
    {
        init_tension_status_bar_for_window(s_main_window);
    }

    window_stack_push(s_main_window, true);
}

void categories_ui_deinit(void)
{
    if (s_registration)
    {
        data_manager_unregister_window_requirements(s_registration);
        s_registration = NULL;
    }
    text_layer_destroy(status_bar_background_layer);
    text_layer_destroy(status_bar_text_layer);

    menu_handler_destroy_window(s_main_window);
}

void request_categories_menu_update(void)
{
    if (!s_main_window)
    {
        return;
    }

    if (status_bar_text_layer)
    {
        static char tension_text[64];
        StringId tensionIndexStringId = PBL_IF_ROUND_ELSE(STRING_TENSION_INDEX_SHORT, STRING_TENSION_INDEX);
        snprintf(tension_text, sizeof(tension_text), "%s: %d°", localization_get_string(tensionIndexStringId),
                 get_tension_index_data()->tension_index);
        text_layer_set_text(status_bar_text_layer, tension_text);
    }
    menu_handler_request_update(s_main_window);
}
