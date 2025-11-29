#include "splash_screen.h"
#include "categories.h"

#ifdef PBL_COLOR
#define BACKGROUND_COLOR GColorRajah
#else
#define BACKGROUND_COLOR GColorWhite
#endif
#define LOGO_TOP_MARGIN 15

static Window *s_window;
static Layer *s_canvas_layer;
static bool s_is_showing = false;

static GBitmap *s_kagi_news_logo_bitmap = NULL;
static GBitmap *s_kagi_doggo_bitmap = NULL;
static GFont s_kagi_news_logo_font = NULL;
static GFont s_loading_font = NULL;

static void canvas_update_proc(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);

    graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    // News logo
    if (s_kagi_news_logo_bitmap)
    {
        GRect bitmap_bounds = gbitmap_get_bounds(s_kagi_news_logo_bitmap);
        GPoint center = GPoint((bounds.size.w - bitmap_bounds.size.w) / 2 - 4, LOGO_TOP_MARGIN);
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, s_kagi_news_logo_bitmap,
                                     GRect(center.x, center.y, bitmap_bounds.size.w, bitmap_bounds.size.h));
    }

    // Doggo image
    if (s_kagi_doggo_bitmap)
    {
        GRect bitmap_bounds = gbitmap_get_bounds(s_kagi_doggo_bitmap);
        GPoint center = GPoint((bounds.size.w - bitmap_bounds.size.w) / 2, (bounds.size.h - bitmap_bounds.size.h));
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, s_kagi_doggo_bitmap,
                                     GRect(center.x, center.y, bitmap_bounds.size.w, bitmap_bounds.size.h));
    }

    graphics_context_set_text_color(ctx, GColorBlack);

    // News logo text
    graphics_draw_text(ctx, "kagi       news", s_kagi_news_logo_font, GRect(0, LOGO_TOP_MARGIN, bounds.size.w, 24),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    // Loading text
    graphics_draw_text(ctx, "Loading...", s_loading_font, GRect(0, (bounds.size.h / 2) - 30, bounds.size.w, 22),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(window_layer, s_canvas_layer);

    s_kagi_news_logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KAGI_NEWS_25);
    s_kagi_doggo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_KAGI_DOGGO_80);
    s_kagi_news_logo_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LUFGA_BLACK_20));
    s_loading_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LUFGA_REGULAR_18));
}

static void window_unload(Window *window)
{
    if (s_kagi_news_logo_bitmap)
    {
        gbitmap_destroy(s_kagi_news_logo_bitmap);
        s_kagi_news_logo_bitmap = NULL;
    }

    if (s_kagi_doggo_bitmap)
    {
        gbitmap_destroy(s_kagi_doggo_bitmap);
        s_kagi_doggo_bitmap = NULL;
    }

    if (s_kagi_news_logo_font)
    {
        fonts_unload_custom_font(s_kagi_news_logo_font);
        s_kagi_news_logo_font = NULL;
    }

    if (s_loading_font)
    {
        fonts_unload_custom_font(s_loading_font);
        s_loading_font = NULL;
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
