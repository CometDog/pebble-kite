#include "menu_handler.h"
#include "ui_config.h"

typedef struct
{
    MenuLayer *menu_layer;
    MenuConfig config;
    bool use_multiline;
    bool never_multiline;
} MenuData;

// Track active menu windows to detect stale pointers
static Window *s_tracked_windows[UI_MAX_TRACKED_WINDOWS];
static int s_tracked_count = 0;

static void track_window(Window *window)
{
    if (s_tracked_count < UI_MAX_TRACKED_WINDOWS)
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
    MenuData *data = callback_context;
    if (data->config.get_num_sections)
    {
        return data->config.get_num_sections();
    }
    return 1;
}

static uint16_t get_number_of_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    MenuData *data = callback_context;
    if (data->config.get_num_items_in_section)
    {
        return data->config.get_num_items_in_section(section_index);
    }
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

static void get_cell_items(MenuData *data, MenuIndex *cell_index, char ***items_out, uint16_t *count_out,
                           bool *is_read_out)
{
    if (data->config.get_items_in_section)
    {
        *items_out = data->config.get_items_in_section(cell_index->section);
        *count_out = data->config.get_num_items_in_section(cell_index->section);
        if (data->config.is_item_read_in_section)
        {
            *is_read_out = data->config.is_item_read_in_section(cell_index->section, cell_index->row);
        }
    }
    else
    {
        *items_out = data->config.get_items();
        *count_out = data->config.get_num_items();
        if (data->config.is_item_read)
        {
            *is_read_out = data->config.is_item_read(cell_index->row);
        }
    }
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;
    char **items;
    uint16_t count;
    bool is_read = false;

    get_cell_items(data, cell_index, &items, &count, &is_read);

    if (cell_index->row < count)
    {
        GRect bounds = layer_get_bounds(cell_layer);
        int16_t content_width = bounds.size.w - (UI_MENU_X_PADDING * 2);

        GFont font = ui_get_system_font_menu();
        if (is_read)
        {
            font = ui_get_system_font_menu_read();
        }

        // Measure actual text height
        GSize text_size = graphics_text_layout_get_content_size(
            items[cell_index->row], font, GRect(0, 0, content_width, bounds.size.h), GTextOverflowModeTrailingEllipsis,
            UI_MENU_TEXT_ALIGNMENT);

        int16_t y_offset = (bounds.size.h - text_size.h) / 2 - 4;
        GRect text_bounds =
            GRect(bounds.origin.x + UI_MENU_X_PADDING, bounds.origin.y + y_offset, content_width, text_size.h);

        graphics_draw_text(ctx, items[cell_index->row], font, text_bounds, GTextOverflowModeTrailingEllipsis,
                           UI_MENU_TEXT_ALIGNMENT, NULL);
    }
}

static void draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context)
{
    MenuData *data = callback_context;
    GRect bounds = layer_get_bounds(cell_layer);
    int16_t content_width = bounds.size.w - (UI_MENU_HEADER_X_PADDING * 2);

    // Draw header background
    graphics_context_set_fill_color(ctx, UI_COLOR_MENU_HEADER_BACKGROUND);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    const char *title;
    if (data->config.get_section_title)
    {
        title = data->config.get_section_title(section_index);
    }
    else
    {
        title = data->config.title;
    }

    // Draw header text with ContentSize-aware font
    graphics_context_set_text_color(ctx, UI_COLOR_MENU_HEADER_TEXT);
    GRect text_bounds = GRect(bounds.origin.x + UI_MENU_HEADER_X_PADDING, bounds.size.h - UI_LINE_HEIGHT_REGULAR,
                              content_width, bounds.size.h);
    graphics_draw_text(ctx, title, ui_get_system_font_menu_title(), text_bounds, GTextOverflowModeTrailingEllipsis,
                       UI_MENU_TEXT_ALIGNMENT, NULL);
}

static int16_t get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    return UI_LINE_HEIGHT_REGULAR;
}

static int16_t get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;

    if (!data->use_multiline)
    {
        return UI_MENU_CELL_HEIGHT_DEFAULT + UI_MENU_CELL_PADDING_Y;
    }

    char **items;
    uint16_t count;
    bool is_read = false;

    get_cell_items(data, cell_index, &items, &count, &is_read);

    if (cell_index->row >= count)
    {
        return UI_MENU_CELL_HEIGHT_DEFAULT + UI_MENU_CELL_PADDING_Y;
    }

    GRect bounds = layer_get_bounds(menu_layer_get_layer(menu_layer));
    int16_t available_width = bounds.size.w - (UI_MENU_X_PADDING * 2);

    // Calculate amount of space the text would take
    GFont font = ui_get_system_font_menu();
    if (is_read)
    {
        font = ui_get_system_font_menu_read();
    }
    GSize text_size =
        graphics_text_layout_get_content_size(items[cell_index->row], font, GRect(0, 0, available_width, 1000),
                                              GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);

    int16_t line_height = UI_LINE_HEIGHT_REGULAR;
    int num_lines = (text_size.h + line_height - 1) / line_height;

    // 1-2 lines: use default cell height
    // 3 lines: use 1.5x default cell height
    // 4+ lines: use 2x default cell height
    if (num_lines >= 4)
    {
        return UI_MENU_CELL_HEIGHT_FOUR_LINE + UI_MENU_CELL_PADDING_Y;
    }
    else if (num_lines >= 3)
    {
        return UI_MENU_CELL_HEIGHT_THREE_LINE + UI_MENU_CELL_PADDING_Y;
    }

    return UI_MENU_CELL_HEIGHT_DEFAULT + UI_MENU_CELL_PADDING_Y;
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    MenuData *data = callback_context;
    if (data->config.select_callback_in_section)
    {
        data->config.select_callback_in_section(cell_index->section, cell_index->row);
    }
    else if (data->config.select_callback)
    {
        data->config.select_callback(cell_index->row);
    }
}

Window *s_menu_handler_create_window(MenuConfig config, bool never_multiline)
{
    Window *window = window_create();
    if (!window)
        return NULL;

    MenuData *menu_data = malloc(sizeof(MenuData));
    if (!menu_data)
    {
        window_destroy(window);
        return NULL;
    }

    menu_data->config = config;
    menu_data->never_multiline = never_multiline;

    bool exceeds_length = false;
    if (!never_multiline)
    {
        if (config.get_num_sections)
        {
            uint16_t num_sections = config.get_num_sections();
            for (uint16_t section = 0; section < num_sections && !exceeds_length; section++)
            {
                char **items = config.get_items_in_section(section);
                uint16_t count = config.get_num_items_in_section(section);
                exceeds_length = any_item_exceeds_length(items, count, 30);
            }
        }
        else
        {
            exceeds_length = any_item_exceeds_length(config.get_items(), config.get_num_items(), 30);
        }
    }
    menu_data->use_multiline = exceeds_length;

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

    menu_layer_set_normal_colors(menu_data->menu_layer, GColorWhite, UI_COLOR_TEXT_PRIMARY);
    menu_layer_set_highlight_colors(menu_data->menu_layer, UI_COLOR_HIGHLIGHT, UI_COLOR_TEXT_ON_DARK);

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
    if (menu_data && menu_data->config.get_num_items && menu_data->config.get_num_items() == 1)
    {
        // If there's now only one item, auto-select it
        if (menu_data->config.select_callback)
        {
            menu_data->config.select_callback(0);
            return;
        }
    }

    if (menu_data && menu_data->menu_layer)
    {
        bool exceeds_length = false;
        if (menu_data->config.get_num_sections)
        {
            uint16_t num_sections = menu_data->config.get_num_sections();
            for (uint16_t section = 0; section < num_sections && !exceeds_length; section++)
            {
                char **items = menu_data->config.get_items_in_section(section);
                uint16_t count = menu_data->config.get_num_items_in_section(section);
                exceeds_length = any_item_exceeds_length(items, count, 20);
            }
        }
        else
        {
            exceeds_length =
                any_item_exceeds_length(menu_data->config.get_items(), menu_data->config.get_num_items(), 20);
        }

        menu_data->use_multiline = menu_data->never_multiline ? false : exceeds_length;
        menu_layer_reload_data(menu_data->menu_layer);
    }
}

void menu_handler_update_sections(Window *window, SectionInfo section_info)
{
    if (!window)
        return;

    if (!is_window_tracked(window))
        return;

    MenuData *menu_data = window_get_user_data(window);
    if (menu_data && menu_data->menu_layer)
    {
        // Currently only single section menus are supported
        if (section_info.section_index == 0)
        {
            menu_data->config.title = section_info.title;
            menu_layer_reload_data(menu_data->menu_layer);
        }
    }
}

void menu_handler_update_title(Window *window, const char *new_title)
{
    if (!window)
        return;

    MenuData *menu_data = window_get_user_data(window);
    if (menu_data)
    {
        menu_data->config.title = new_title;
        if (menu_data->menu_layer)
        {
            menu_layer_reload_data(menu_data->menu_layer);
        }
    }
}

void menu_handler_set_is_read_callback(Window *window, bool (*callback)(int index))
{
    if (!window)
        return;

    MenuData *menu_data = window_get_user_data(window);
    if (menu_data)
    {
        menu_data->config.is_item_read = callback;
    }
}