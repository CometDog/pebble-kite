#include "qr_view.h"
#include "../data.h"
#include "../data_manager.h"
#include "../localization/localization.h"
#include "../utils/debug_logger.h"
#include "ui_config.h"

static Window *s_qr_window;
static DataRegistrationHandle s_registration = NULL;

static void draw_micro_qr(GContext *ctx)
{
    const QRCodeData *qr_data = get_qr_code_data();
    const uint8_t *bytes = qr_data->qr_code_chunks;
    const int width = qr_data->qr_size;

    if (width == 0)
    {
        ERROR_LOG("KNCQRView", "draw_micro_qr : QR size is 0, cannot draw");
        return;
    }

    // Get screen bounds to calculate best scale
    GRect screen_bounds = layer_get_bounds(window_get_root_layer(s_qr_window));
#ifdef PBL_ROUND
    // Determine the largest square that fits within the round screen
    const int safe_size = (screen_bounds.size.w / 1.41421356237); // Approximation of sqrt(2)
    GRect bounds =
        GRect((screen_bounds.size.w - safe_size) / 2, (screen_bounds.size.h - safe_size) / 2, safe_size, safe_size);
#else
    GRect bounds = screen_bounds;
    bounds.size.h -= UI_QR_Y_FUDGE;
#endif
    const int min_screen_dim = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;

    // Calculate max scale that fits on screen
    int scale = 4;
    while (scale > 1 && (width * scale) > min_screen_dim)
    {
        scale--;
    }

    if (scale == 0)
    {
        ERROR_LOG("KNCQRView", "draw_micro_qr : Cannot fit QR code on screen");
        return;
    }

    // Center the QR code within `bounds`
    const int qr_total_size = width * scale;
    const int offset_x = (bounds.size.w - qr_total_size) / 2;
    const int offset_y = (bounds.size.h - qr_total_size) / 2;

    DEBUG_LOG("KNCQRView", "width=%d, scale=%d, size=%d, offset=(%d,%d)", width, scale, qr_total_size, offset_x,
              offset_y);

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
                GRect rect = GRect(bounds.origin.x + offset_x + (x * scale), bounds.origin.y + offset_y + (y * scale),
                                   scale, scale);
                graphics_context_set_fill_color(ctx, UI_COLOR_TEXT_PRIMARY);
                graphics_fill_rect(ctx, rect, 0, GCornerNone);
            }
        }
    }

    // Draw the domain text below the QR code
    const DetailTextData *detail_data = get_detail_text_data();
    if (detail_data && detail_data->detail_title)
    {
        GFont font = ui_get_system_font_caption();
        GRect text_bounds;
#ifdef PBL_ROUND
        // Place the caption in the chin area below the safe box
        const int chin_padding = 32;
        const int text_x = bounds.origin.x + chin_padding;
        const int text_w = bounds.size.w - (chin_padding * 2);
        const int text_y = bounds.origin.y + bounds.size.h;
        const int text_h = UI_QR_Y_FUDGE;
        text_bounds = GRect(text_x, text_y, text_w, text_h);
#else
        text_bounds = GRect(bounds.origin.x, bounds.origin.y + bounds.size.h, bounds.size.w, UI_QR_Y_FUDGE);
#endif
        graphics_context_set_text_color(ctx, UI_COLOR_TEXT_PRIMARY);
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
        GFont font = ui_get_system_font_title();
        GTextAlignment align = GTextAlignmentCenter;
        GRect text_bounds =
            GRect(0, (bounds.size.h / 2) - (UI_QR_LOADING_TEXT_HEIGHT / 2), bounds.size.w, UI_QR_LOADING_TEXT_HEIGHT);
        graphics_context_set_text_color(ctx, UI_COLOR_TEXT_PRIMARY);
        graphics_draw_text(ctx, localization_get_string(STRING_LOADING), font, text_bounds,
                           GTextOverflowModeTrailingEllipsis, align, NULL);
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
