#include "menu_handler.h"

#define MENU_X_PADDING 6
#define MAX_TRACKED_WINDOWS 16
#define CELL_PADDING_Y 4
#define DEFAULT_CELL_HEIGHT 40
#define ONE_LINE_MULTI_LINE_CELL_HEIGHT (DEFAULT_CELL_HEIGHT / 2)
#define THREE_LINE_CELL_HEIGHT (DEFAULT_CELL_HEIGHT * 3 / 2)
#define FOUR_LINE_CELL_HEIGHT (DEFAULT_CELL_HEIGHT * 2)

typedef struct
{
    MenuLayer *menu_layer;
    MenuConfig config;
    bool use_multiline;
    bool never_multiline;
} MenuData;

// Track active menu windows to detect stale pointers
static Window *s_tracked_windows[MAX_TRACKED_WINDOWS];
static int s_tracked_count = 0;
static GFont s_menu_font = NULL;

static void track_window(Window *window)
{
    if (s_tracked_count < MAX_TRACKED_WINDOWS)
    {
        s_tracked_windows[s_tracked_count++] = window;
    }
}

static void untrack_window(Window *window)
{
    for (int i = 0; i < s_tracked_count; i++)
    {
        if (s_tracked_windows[i] == window)
        {
            // Shift remaining windows down
            for (int j = i; j < s_tracked_count - 1; j++)
            {
                s_tracked_windows[j] = s_tracked_windows[j + 1];
            }
            s_tracked_count--;
            return;
        }
    }
}

static bool is_window_tracked(Window *window)
{
    for (int i = 0; i < s_tracked_count; i++)
    {
        if (s_tracked_windows[i] == window)
        {
            return true;
        }
    }
    return false;
}

static uint16_t get_number_of_sections(struct MenuLayer *menu_layer, void *callback_context)
{
    return 1;
}

static uint16_t get_number_of_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    MenuData *data = callback_context;
    return data->config.get_num_items();
}

static bool any_item_exceeds_length(char **items, uint16_t count, size_t max_length)
{
    for (uint16_t i = 0; i < count; i++)
    {
        if (strlen(items[i]) >= max_length)
        {
            return true;
        }
    }
    return false;
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;
    char **items = data->config.get_items();
    uint16_t count = data->config.get_num_items();

    if (cell_index->row < count)
    {
        if (!data->use_multiline)
        {
            menu_cell_basic_draw(ctx, cell_layer, items[cell_index->row], NULL, NULL);
        }
        else
        {
            GRect bounds = layer_get_bounds(cell_layer);
            GRect boundsWithPadding = GRect(bounds.origin.x + MENU_X_PADDING, bounds.origin.y,
                                            bounds.size.w - (MENU_X_PADDING * 2), bounds.size.h);
            graphics_draw_text(ctx, items[cell_index->row], s_menu_font, boundsWithPadding,
                               GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
        }
    }
}

static void draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context)
{
    MenuData *data = callback_context;
    menu_cell_basic_header_draw(ctx, cell_layer, data->config.title);
}

static int16_t get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;

    if (!data->use_multiline)
    {
        return DEFAULT_CELL_HEIGHT + CELL_PADDING_Y;
    }

    char **items = data->config.get_items();
    uint16_t count = data->config.get_num_items();

    if (cell_index->row >= count)
    {
        return DEFAULT_CELL_HEIGHT + CELL_PADDING_Y;
    }

    GRect bounds = layer_get_bounds(menu_layer_get_layer(menu_layer));
    int16_t available_width = bounds.size.w - (MENU_X_PADDING * 2);

    // Calculate amount of space the text would take
    GSize text_size =
        graphics_text_layout_get_content_size(items[cell_index->row], s_menu_font, GRect(0, 0, available_width, 1000),
                                              GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

    // 18 is about the line height of Gothic 18
    int16_t line_height = 18;
    int num_lines = (text_size.h + line_height - 1) / line_height;

    // 1-2 lines: use default cell height
    // 3 lines: use 1.5x default cell height
    // 4+ lines: use 2x default cell height
    if (num_lines >= 4)
    {
        return FOUR_LINE_CELL_HEIGHT + CELL_PADDING_Y;
    }
    else if (num_lines >= 3)
    {
        return THREE_LINE_CELL_HEIGHT + CELL_PADDING_Y;
    }
    else if (num_lines >= 2)
    {
        return DEFAULT_CELL_HEIGHT + CELL_PADDING_Y;
    }
    else if (num_lines == 1)
    {
        return ONE_LINE_MULTI_LINE_CELL_HEIGHT + CELL_PADDING_Y;
    }

    return DEFAULT_CELL_HEIGHT + CELL_PADDING_Y;
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;
    if (data->config.select_callback)
    {
        data->config.select_callback(cell_index->row);
    }
}

Window *s_menu_handler_create_window(MenuConfig config, bool never_multiline)
{
    Window *window = window_create();
    if (!window)
        return NULL;

    if (!s_menu_font)
    {
        s_menu_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    }

    MenuData *menu_data = malloc(sizeof(MenuData));
    if (!menu_data)
    {
        window_destroy(window);
        return NULL;
    }

    menu_data->config = config;
    menu_data->never_multiline = never_multiline;
    menu_data->use_multiline =
        never_multiline ? false : any_item_exceeds_length(config.get_items(), config.get_num_items(), 30);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    menu_data->menu_layer = menu_layer_create(bounds);
    if (!menu_data->menu_layer)
    {
        free(menu_data);
        window_destroy(window);
        return NULL;
    }

    menu_layer_set_callbacks(menu_data->menu_layer, menu_data,
                             (MenuLayerCallbacks){.get_num_sections = get_number_of_sections,
                                                  .get_num_rows = get_number_of_rows,
                                                  .get_cell_height = get_cell_height,
                                                  .draw_row = draw_row,
                                                  .draw_header = draw_header,
                                                  .get_header_height = get_header_height,
                                                  .select_click = select_callback});

    menu_layer_set_click_config_onto_window(menu_data->menu_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(menu_data->menu_layer));

    window_set_user_data(window, menu_data);

    track_window(window);
    return window;
}

Window *menu_handler_create_window_no_multiline(MenuConfig config)
{
    return s_menu_handler_create_window(config, true);
}

Window *menu_handler_create_window(MenuConfig config)
{
    return s_menu_handler_create_window(config, false);
}

void menu_handler_destroy_window(Window *window)
{
    if (!window)
        return;

    untrack_window(window);

    window_set_window_handlers(window, (WindowHandlers){0});

    MenuData *menu_data = window_get_user_data(window);
    if (menu_data)
    {
        if (menu_data->menu_layer)
        {
            menu_layer_destroy(menu_data->menu_layer);
        }
        free(menu_data);
        window_set_user_data(window, NULL);
    }
    window_destroy(window);
}

void menu_handler_request_update(Window *window)
{
    if (!window)
        return;

    if (!is_window_tracked(window))
        return;

    MenuData *menu_data = window_get_user_data(window);
    if (menu_data && menu_data->menu_layer)
    {
        menu_data->use_multiline =
            menu_data->never_multiline
                ? false
                : any_item_exceeds_length(menu_data->config.get_items(), menu_data->config.get_num_items(), 20);
        menu_layer_reload_data(menu_data->menu_layer);
    }
}