#include "detail_text.h"
#include "../data.h"
#include "../localization/localization.h"
#include "ui_config.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    Window *window;
    ScrollLayer *scroll_layer;
    TextLayer *title_layer;
    TextLayer *text_layer;
    Layer *indicator_layer;
    char *(*get_title)(void);
    char *(*get_text)(void);
    DataRegistrationHandle registration;
    void (*on_unload)(void);
    void (*on_select)(void);
} DetailTextContext;

static DetailTextContext s_contexts[UI_MAX_DETAIL_CONTEXTS];

static DetailTextContext *get_context_for_window(Window *w)
{
    for (int i = 0; i < UI_MAX_DETAIL_CONTEXTS; ++i)
    {
        if (s_contexts[i].window == w)
            return &s_contexts[i];
    }
    return NULL;
}

static DetailTextContext *allocate_context(void)
{
    for (int i = 0; i < UI_MAX_DETAIL_CONTEXTS; ++i)
    {
        if (s_contexts[i].window == NULL)
            return &s_contexts[i];
    }
    return NULL;
}

static void free_context(DetailTextContext *ctx)
{
    if (!ctx)
        return;
    ctx->window = NULL;
    ctx->scroll_layer = NULL;
    ctx->title_layer = NULL;
    ctx->text_layer = NULL;
    ctx->indicator_layer = NULL;
    ctx->get_title = NULL;
    ctx->get_text = NULL;
    ctx->registration = NULL;
    ctx->on_unload = NULL;
    ctx->on_select = NULL;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
    Window *window = (Window *)context;
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    if (ctx->on_select)
    {
        ctx->on_select();
    }
}

static void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void indicator_layer_update_proc(Layer *layer, GContext *ctx)
{
    // Logic attempts to mirror that of the firmware handling of the action button indicator
    GRect bounds = layer_get_bounds(layer);

    GRect indicator_bounds = GRect(0, 0, UI_INDICATOR_RADIUS * 2, UI_INDICATOR_RADIUS * 2);
    grect_align(&indicator_bounds, &bounds, GAlignRight, false);
    indicator_bounds.origin.x += UI_INDICATOR_X_OFFSET;

    graphics_context_set_fill_color(ctx, UI_COLOR_TEXT_PRIMARY);
    graphics_fill_circle(
        ctx, GPoint(indicator_bounds.origin.x + UI_INDICATOR_RADIUS, indicator_bounds.origin.y + UI_INDICATOR_RADIUS),
        UI_INDICATOR_RADIUS);
}

static void update_content_for_window(Window *window)
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    const char *title = ctx->get_title ? ctx->get_title() : "";
    const char *text = ctx->get_text ? ctx->get_text() : "";

    Layer *scroll_layer_l = scroll_layer_get_layer(ctx->scroll_layer);
    GRect scroll_bounds = layer_get_bounds(scroll_layer_l);

    int side_padding = UI_DETAIL_SIDE_PADDING + UI_DETAIL_SIDE_PADDING_EXTRA;
    const int between_padding = UI_DETAIL_BETWEEN_PADDING;
    int bottom_padding = UI_DETAIL_BOTTOM_PADDING;

    int content_width = scroll_bounds.size.w - side_padding * 2;
    GRect measure_bounds = GRect(0, 0, content_width, 30000);

    GFont title_font = ui_get_system_font_title();
    GFont body_font = ui_get_system_font_body();

    GSize title_size = graphics_text_layout_get_content_size(
        title, title_font, measure_bounds, GTextOverflowModeTrailingEllipsis, UI_DETAIL_TEXT_ALIGNMENT);
    GSize text_size = graphics_text_layout_get_content_size(text, body_font, measure_bounds, GTextOverflowModeWordWrap,
                                                            UI_DETAIL_TEXT_ALIGNMENT);

    // Small fudge to avoid rounding errors
    const int safety_pixels = UI_TEXT_SAFETY_PIXELS;
    int title_h = title_size.h + safety_pixels;
    int text_h = text_size.h + safety_pixels;

    // Title
    int title_x = side_padding;
    int title_y = UI_DETAIL_TITLE_Y_INITIAL(title_h, side_padding, scroll_bounds.size.h);
    layer_set_frame(text_layer_get_layer(ctx->title_layer), GRect(title_x, title_y, content_width, title_h));
    text_layer_set_font(ctx->title_layer, title_font);
    text_layer_set_text(ctx->title_layer, title);

    // Text
    int text_x = side_padding;
    int text_y = title_y + title_h + between_padding;
    layer_set_frame(text_layer_get_layer(ctx->text_layer), GRect(text_x, text_y, content_width, text_h));
    text_layer_set_font(ctx->text_layer, body_font);
    text_layer_set_text(ctx->text_layer, text);

    // Content size for scroll layer (ensure at least window height)
    int content_height = text_y + text_h + bottom_padding + UI_DETAIL_BOTTOM_PADDING_FOR_ROUND(scroll_bounds.size.h);
    if (content_height < scroll_bounds.size.h)
    {
        content_height = scroll_bounds.size.h;
    }
    scroll_layer_set_content_size(ctx->scroll_layer, GSize(scroll_bounds.size.w, content_height));

    scroll_layer_set_content_offset(ctx->scroll_layer, GPoint(0, 0), false);
}

static void detail_window_load(Window *window)
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // TODO: Remove this. Current there is a bug in the firmware where width can't be greater than 255
    if (bounds.size.w > 255)
    {
        bounds.origin.x = bounds.size.w - 255;
        bounds.size.w = 255;
    }
    ctx->scroll_layer = scroll_layer_create(bounds);
    scroll_layer_set_click_config_onto_window(ctx->scroll_layer, window);
    scroll_layer_set_callbacks(ctx->scroll_layer,
                               (ScrollLayerCallbacks){.click_config_provider = click_config_provider});
    scroll_layer_set_context(ctx->scroll_layer, window);

    ctx->title_layer =
        text_layer_create(GRect(UI_DETAIL_SIDE_PADDING, 0, bounds.size.w - UI_DETAIL_SIDE_PADDING * 2, 30));
    text_layer_set_text(ctx->title_layer, "Loading title...");
    text_layer_set_font(ctx->title_layer, ui_get_system_font_title());
    text_layer_set_overflow_mode(ctx->title_layer, GTextOverflowModeTrailingEllipsis);
    text_layer_set_text_alignment(ctx->title_layer, UI_DETAIL_TEXT_ALIGNMENT);

    ctx->text_layer =
        text_layer_create(GRect(UI_DETAIL_SIDE_PADDING, 0, bounds.size.w - UI_DETAIL_SIDE_PADDING * 2, 30));
    text_layer_set_text(ctx->text_layer, localization_get_string(STRING_LOADING));
    text_layer_set_font(ctx->text_layer, ui_get_system_font_body());
    text_layer_set_overflow_mode(ctx->text_layer, GTextOverflowModeWordWrap);
    text_layer_set_text_alignment(ctx->text_layer, UI_DETAIL_TEXT_ALIGNMENT);

    scroll_layer_add_child(ctx->scroll_layer, text_layer_get_layer(ctx->title_layer));
    scroll_layer_add_child(ctx->scroll_layer, text_layer_get_layer(ctx->text_layer));
    scroll_layer_set_content_size(ctx->scroll_layer, GSize(bounds.size.w, 60));
    layer_add_child(window_layer, scroll_layer_get_layer(ctx->scroll_layer));

    ctx->indicator_layer = layer_create(bounds);
    layer_set_update_proc(ctx->indicator_layer, indicator_layer_update_proc);
    layer_set_hidden(ctx->indicator_layer, !ctx->on_select);
    layer_add_child(window_layer, ctx->indicator_layer);
}

static void detail_window_appear(Window *window)
{
    update_content_for_window(window);
}

static void detail_window_disappear(Window *window)
{
    // No-op
}

static void detail_window_unload(Window *window)
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;

    if (ctx->text_layer)
    {
        text_layer_destroy(ctx->text_layer);
        ctx->text_layer = NULL;
    }
    if (ctx->title_layer)
    {
        text_layer_destroy(ctx->title_layer);
        ctx->title_layer = NULL;
    }
    if (ctx->scroll_layer)
    {
        scroll_layer_destroy(ctx->scroll_layer);
        ctx->scroll_layer = NULL;
    }
    if (ctx->indicator_layer)
    {
        layer_destroy(ctx->indicator_layer);
        ctx->indicator_layer = NULL;
    }

    if (ctx->registration)
    {
        data_manager_unregister_window_requirements(ctx->registration);
        ctx->registration = NULL;
    }

    if (ctx->on_unload)
    {
        ctx->on_unload();
    }

    free_context(ctx);
}

Window *detail_text_create(Window **window, char *(*get_title)(void), char *(*get_text)(void), const char *detail_type,
                           DataResource *resources, size_t resource_count, DataRegistrationHandle *handle)
{
    return detail_text_create_with_unload(window, get_title, get_text, detail_type, resources, resource_count, handle,
                                          NULL);
}

Window *detail_text_create_with_unload(Window **window, char *(*get_title)(void), char *(*get_text)(void),
                                       const char *detail_type, DataResource *resources, size_t resource_count,
                                       DataRegistrationHandle *handle, void (*on_unload)(void))
{
    if (!window || !get_title || !get_text)
        return NULL;
    DetailTextContext *ctx = allocate_context();
    if (!ctx)
        return NULL;
    Window *content = window_create();
    if (!content)
    {
        free_context(ctx);
        return NULL;
    }
    ctx->window = content;
    ctx->get_title = get_title;
    ctx->get_text = get_text;
    ctx->registration = NULL;
    ctx->on_unload = on_unload;
    ctx->on_select = NULL;
    // detail_type parameter is kept for future use but not stored in context
    // The global detail_type state is set by the select handler that creates this window
    (void)detail_type;

    window_set_window_handlers(content, (WindowHandlers){.load = detail_window_load,
                                                         .unload = detail_window_unload,
                                                         .appear = detail_window_appear,
                                                         .disappear = detail_window_disappear});

    if (resources && resource_count > 0)
    {
        ctx->registration = data_manager_register_window_requirements(content, resources, resource_count);
        if (handle)
        {
            *handle = ctx->registration;
        }
    }

    *window = content;
    window_stack_push(content, true);
    return content;
}

void detail_text_destroy(Window *window)
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    if (ctx->registration)
    {
        data_manager_unregister_window_requirements(ctx->registration);
        ctx->registration = NULL;
    }

    window_set_window_handlers(window, (WindowHandlers){0});
    window_destroy(window);
    free_context(ctx);
}

void detail_text_update(Window *window)
{
    if (!window)
        return;
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    if (window_stack_get_top_window() != window)
        return;
    update_content_for_window(window);
}

void detail_text_set_select_handler(Window *window, void (*on_select)(void))
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx)
        return;
    ctx->on_select = on_select;
    if (ctx->indicator_layer)
    {
        layer_set_hidden(ctx->indicator_layer, !on_select);
    }
}

void detail_text_set_scroll_handler(Window *window, ScrollLayerCallback callback)
{
    DetailTextContext *ctx = get_context_for_window(window);
    if (!ctx || !ctx->scroll_layer)
        return;

    // Preserve existing click config provider
    scroll_layer_set_callbacks(ctx->scroll_layer, (ScrollLayerCallbacks){.click_config_provider = click_config_provider,
                                                                         .content_offset_changed_handler = callback});
}