#include "./detail_list_window.h"
#include "../communication.h"
#include "../data.h"
#include "../data_manager.h"
#include "../detail_types.h"
#include "../localization/localization.h"
#include "./menu_handler.h"
#include "./navigation.h"
#include "detail_list.h"
#include <string.h>

// Per-window context data
typedef struct
{
    Window *window; // Back-reference to the window
    DetailType detail_type;
    DetailListSelectionCallback on_select;
    DataRegistrationHandle registration;
} DetailListWindowContext;

// Map windows to their contexts (since menu_handler uses window_set_user_data)
#define MAX_DETAIL_WINDOWS 8
static DetailListWindowContext *s_window_contexts[MAX_DETAIL_WINDOWS] = {0};

static DetailListWindowContext *get_window_context(Window *window)
{
    for (int i = 0; i < MAX_DETAIL_WINDOWS; i++)
    {
        if (s_window_contexts[i] && s_window_contexts[i]->window == window)
        {
            return s_window_contexts[i];
        }
    }
    return NULL;
}

static void register_window_context(DetailListWindowContext *ctx)
{
    for (int i = 0; i < MAX_DETAIL_WINDOWS; i++)
    {
        if (s_window_contexts[i] == NULL)
        {
            s_window_contexts[i] = ctx;
            return;
        }
    }
}

static void unregister_window_context(Window *window)
{
    for (int i = 0; i < MAX_DETAIL_WINDOWS; i++)
    {
        if (s_window_contexts[i] && s_window_contexts[i]->window == window)
        {
            s_window_contexts[i] = NULL;
            return;
        }
    }
}

// Helper wrappers for story titles
static uint16_t get_story_titles_count_wrapper(void)
{
    const StoryListData *data = get_story_list_data();
    return data ? data->story_titles_count : 0;
}
static char **get_story_titles_wrapper(void)
{
    const StoryListData *data = get_story_list_data();
    return data ? data->story_titles : NULL;
}

// Helper wrappers for detail options
static uint16_t get_detail_options_count_wrapper(void)
{
    const DetailListData *data = get_detail_list_data();
    return data ? data->detail_options_count : 0;
}
static char **get_detail_options_wrapper(void)
{
    const DetailListData *data = get_detail_list_data();
    return data ? data->detail_options : NULL;
}

// Helper wrappers for detail sources
static uint16_t get_detail_sources_count_wrapper(void)
{
    const DetailSourcesData *data = get_detail_sources_data();
    return data ? data->sources_count : 0;
}
static char **get_detail_sources_wrapper(void)
{
    const DetailSourcesData *data = get_detail_sources_data();
    return data ? data->sources : NULL;
}

static void detail_list_window_select_handler(int index)
{
    // Get the top window and find its context
    Window *window = window_stack_get_top_window();
    if (!window)
        return;

    DetailListWindowContext *ctx = get_window_context(window);
    if (ctx && ctx->on_select)
    {
        ctx->on_select(index);
    }
}

static void detail_list_window_appear(Window *window)
{
    // Placeholder for any actions needed when the window appears
}

static void detail_list_window_unload(Window *window)
{
    // Free the per-window context
    DetailListWindowContext *ctx = get_window_context(window);
    if (ctx)
    {
        if (ctx->registration)
        {
            data_manager_unregister_window_requirements(ctx->registration);
        }
        unregister_window_context(window);
        free(ctx);
    }

    // Now clean up the window itself (this also cleans up MenuData)
    menu_handler_destroy_window(window);
}

void detail_list_window_ui_init(Window **window, DataResource resource_requirements[], DetailType detail_type,
                                DetailListSelectionCallback on_select)
{
    // Allocate per-window context
    DetailListWindowContext *ctx = malloc(sizeof(DetailListWindowContext));
    if (!ctx)
        return;

    ctx->window = NULL; // Will be set after window creation
    ctx->detail_type = detail_type;
    ctx->on_select = on_select;
    ctx->registration = NULL;

    const char *title;
    if (detail_type == DETAIL_TYPE_PRIMARY_STORY)
    {
        // Special case: use dynamic category name
        title = get_story_list_data()->selected_category;
        if (title == NULL)
        {
            title = localization_get_string(STRING_STORIES);
        }
        else
        {
            title = localization_translate_category(title);
        }
    }
    else if (detail_type == DETAIL_TYPE_DETAIL_SOURCES)
    {
        // Special case: sources for a specific detail
        title = localization_get_string(STRING_AVAILABLE_DETAILS);
    }
    else
    {
        // Use centralized detail_types registry for display name
        title = detail_type_get_display_name_from_type(detail_type);
    }

    uint16_t (*get_options_count)(void);
    char **(*get_options)(void);

    if (detail_type == DETAIL_TYPE_PRIMARY_STORY)
    {
        get_options_count = get_story_titles_count_wrapper;
        get_options = get_story_titles_wrapper;
    }
    else if (detail_type == DETAIL_TYPE_DETAIL_SOURCES)
    {
        get_options_count = get_detail_sources_count_wrapper;
        get_options = get_detail_sources_wrapper;
    }
    else
    {
        get_options_count = get_detail_options_count_wrapper;
        get_options = get_detail_options_wrapper;
    }
    *window = detail_list_create_with_resources(window, title, get_options_count, get_options,
                                                detail_list_window_select_handler, resource_requirements, 1,
                                                &ctx->registration);
    if (window && *window)
    {
        // Store the window pointer in context and register it
        ctx->window = *window;
        register_window_context(ctx);

        // Note: we can't use window_set_user_data because menu_handler uses it for MenuData

        window_set_window_handlers(
            *window, (WindowHandlers){.appear = detail_list_window_appear, .unload = detail_list_window_unload});
        window_stack_push(*window, true);
    }
    else
    {
        // Window creation failed, clean up context
        free(ctx);
    }
}

static void generic_detail_list_select_handler(int index)
{
    const DetailListData *list_data = get_detail_list_data();
    if (!list_data->detail_options || index >= list_data->detail_options_count)
    {
        return;
    }

    const char *option = list_data->detail_options[index];
    if (!option || strcmp(option, "Loading...") == 0)
    {
        return;
    }

    const char *detail_type_str = list_data->detail_type;
    if (!detail_type_str)
    {
        return;
    }

    const DetailTypeInfo *info = detail_type_from_api_name(detail_type_str);
    if (!info)
    {
        return;
    }

    if (info->detail_type == DETAIL_TYPE_ARTICLES)
    {
        // Sources go to QR code view
        set_detail_title((char *)option, NULL);
        navigation_open_qr_code(option);
    }
    else if (info->has_text)
    {
        // Types with text content go to text view
        navigation_open_detail_text(detail_type_str, option);
    }
}

void detail_list_window_ui_init_with_default_nav(Window **window, DetailType detail_type)
{
    DataResource resources[] = {DATA_RESOURCE_STORY_DETAIL_OPTIONS};
    detail_list_window_ui_init(window, resources, detail_type, generic_detail_list_select_handler);
}

void detail_list_window_ui_deinit(Window **window)
{
    // The unload handler takes care of cleanup, just null out the pointer
    if (window && *window)
    {
        *window = NULL;
    }
}

void detail_list_window_ui_update(Window **window)
{
    menu_handler_request_update(*window);
}

void detail_list_window_set_is_read_callback(Window **window, bool (*callback)(int index))
{
    if (!window || !*window)
        return;
    detail_list_set_is_read_callback(*window, callback);
}
