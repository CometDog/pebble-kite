#include "splash_screen.h"
#include "categories.h"
#include "ui_config.h"

static Window *s_window;
static Layer *s_canvas_layer;
static bool s_is_showing = false;
static uint16_t s_loading_current = 0;
static uint16_t s_loading_max = 100;

static GBitmap *s_splash_bitmap = NULL;

static uint16_t get_loading_percent(void)
{
    if (s_loading_max == 0)
    {
        return 0;
    }

    if (s_loading_current >= s_loading_max)
    {
        return 100;
    }

    return (uint16_t)((s_loading_current * 100U) / s_loading_max);
}

static void canvas_update_proc(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);
    UISplashProgressLayout layout = ui_get_splash_progress_layout(bounds);

    graphics_context_set_fill_color(ctx, GColorClear);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    if (s_splash_bitmap)
    {
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, s_splash_bitmap, bounds);
    }

    uint16_t percent = get_loading_percent();
    int16_t fill_width = (int16_t)((layout.bar_frame.size.w * percent) / 100U);

    graphics_context_set_fill_color(ctx, UI_COLOR_SPLASH_PROGRESS_BACKGROUND);
    graphics_fill_rect(ctx, layout.background_frame, layout.background_radius, GCornersAll);

    graphics_context_set_stroke_color(ctx, UI_COLOR_SPLASH_PROGRESS_BORDER);
    graphics_draw_round_rect(ctx, layout.bar_frame, layout.bar_radius);

    if (fill_width > 0)
    {
        graphics_context_set_fill_color(ctx, UI_COLOR_SPLASH_PROGRESS_FILL);
        graphics_fill_rect(ctx,
                           GRect(layout.bar_frame.origin.x + 1, layout.bar_frame.origin.y + 1,
                                 fill_width > 2 ? fill_width - 2 : 1, layout.bar_frame.size.h - 2),
                           2, GCornersAll);
    }

    static char percent_text[8];
    snprintf(percent_text, sizeof(percent_text), "%u%%", (unsigned int)percent);
    graphics_context_set_text_color(ctx, UI_COLOR_SPLASH_PROGRESS_TEXT);
    graphics_draw_text(ctx, percent_text, ui_get_system_font_splash_progress(), layout.label_frame,
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(window_layer, s_canvas_layer);
    s_splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPLASH_SCREEN);
}

static void window_unload(Window *window)
{
    if (s_splash_bitmap)
    {
        gbitmap_destroy(s_splash_bitmap);
        s_splash_bitmap = NULL;
    }

    if (s_canvas_layer)
    {
        layer_destroy(s_canvas_layer);
        s_canvas_layer = NULL;
    }
}

void splash_screen_init(void)
{
    s_window = window_create();
    s_loading_current = 0;
    s_loading_max = 100;

    window_set_window_handlers(s_window, (WindowHandlers){
                                             .load = window_load,
                                             .unload = window_unload,
                                         });

    window_stack_push(s_window, false);
    s_is_showing = true;
}

void splash_screen_deinit(void)
{
    if (s_window)
    {
        window_destroy(s_window);
        s_window = NULL;
    }
    s_is_showing = false;
}

void splash_screen_dismiss(void)
{
    if (!s_is_showing)
    {
        return;
    }

    categories_ui_init();

    if (s_window)
    {
        window_stack_remove(s_window, false);
    }

    s_is_showing = false;
}

bool splash_screen_is_showing(void)
{
    return s_is_showing;
}

void splash_screen_set_loading_progress(uint16_t current, uint16_t max)
{
    if (current > s_loading_current)
    {
        s_loading_current = current;
    }
    s_loading_max = max > 0 ? max : 100;

    if (s_canvas_layer)
    {
        layer_mark_dirty(s_canvas_layer);
    }
}
