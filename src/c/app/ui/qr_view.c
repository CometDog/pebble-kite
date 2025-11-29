#include "qr_view.h"
#include "../data.h"
#include "../data_manager.h"

#define LOADING_TEXT "Loading..."

static Window *s_qr_window;
static DataRegistrationHandle s_registration = NULL;

#define QR_Y_FUDGE 24

static void draw_micro_qr(GContext *ctx)
{
    const QRCodeData *qr_data = get_qr_code_data();
    const uint8_t *bytes = qr_data->qr_code_chunks;
    const int width = qr_data->qr_size;

    if (width == 0)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "[KNCQRView] draw_micro_qr: QR size is 0, cannot draw");
        return;
    }

    // Get screen bounds to calculate best scale
    GRect bounds = layer_get_bounds(window_get_root_layer(s_qr_window));
    const int min_screen_dim = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;

    // Calculate max scale that fits on screen
    int scale = 4;
    while (scale > 1 && (width * scale) > min_screen_dim)
    {
        scale--;
    }

    if (scale == 0)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "[KNCQRView] draw_micro_qr: Cannot fit QR code on screen");
        return;
    }

    // Center the QR code on screen (with fudge for domain text)
    const int qr_total_size = width * scale;
    const int offset_x = (bounds.size.w - qr_total_size) / 2;
    const int offset_y = ((bounds.size.h - QR_Y_FUDGE) - qr_total_size) / 2;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "[KNCQRView] width=%d, scale=%d, size=%d, offset=(%d,%d)", width, scale, qr_total_size,
            offset_x, offset_y);

    // Draw the QR code
    for (int y = 0; y < width; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int bit_index = y * width + x;
            int byte_index = bit_index / 8;
            int bit_position = 7 - (bit_index % 8);

            bool is_black = (bytes[byte_index] >> bit_position) & 1;

            if (is_black)
            {
                GRect rect = GRect(offset_x + (x * scale), offset_y + (y * scale), scale, scale);
                graphics_context_set_fill_color(ctx, GColorBlack);
                graphics_fill_rect(ctx, rect, 0, GCornerNone);
            }
        }
    }

    // Draw the domain text below the QR code
    const DetailTextData *detail_data = get_detail_text_data();
    if (detail_data && detail_data->detail_title)
    {
        GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
        GRect text_bounds = GRect(0, bounds.size.h - QR_Y_FUDGE, bounds.size.w, QR_Y_FUDGE);
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_draw_text(ctx, detail_data->detail_title, font, text_bounds, GTextOverflowModeTrailingEllipsis,
                           GTextAlignmentCenter, NULL);
    }
}

static void qr_layer_update_proc(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    if (!qr_code_loaded())
    {
        GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
        GTextAlignment align = GTextAlignmentCenter;
        GRect text_bounds = GRect(0, (bounds.size.h / 2) - 12, bounds.size.w, 24);
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_draw_text(ctx, LOADING_TEXT, font, text_bounds, GTextOverflowModeTrailingEllipsis, align, NULL);
    }
    else
    {
        draw_micro_qr(ctx);
    }
}

static void qr_window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    layer_set_update_proc(window_layer, qr_layer_update_proc);
}

static void qr_window_unload(Window *window)
{
    qr_view_deinit();
}

void qr_view_init(void)
{
    if (s_qr_window)
        return;

    s_qr_window = window_create();
    window_set_window_handlers(s_qr_window, (WindowHandlers){
                                                .load = qr_window_load,
                                                .unload = qr_window_unload,
                                            });
    DataResource resources[] = {DATA_RESOURCE_QR_CODE};
    s_registration = data_manager_register_window_requirements(s_qr_window, resources, 1);

    window_stack_push(s_qr_window, true);
}

void qr_view_deinit(void)
{
    // Don't manually clear here
    // Let data_manager handle it based on refcounts clear_qr_code_bitmap_chunks();
    if (s_qr_window)
    {
        if (s_registration)
        {
            data_manager_unregister_window_requirements(s_registration);
            s_registration = NULL;
        }

        window_set_window_handlers(s_qr_window, (WindowHandlers){0});
        window_destroy(s_qr_window);
        s_qr_window = NULL;
    }
}

void qr_view_refresh(void)
{
    if (!s_qr_window)
        return;
    Layer *root = window_get_root_layer(s_qr_window);
    if (root)
    {
        layer_mark_dirty(root);
    }
}
