#include "ui_config.h"

// Define the font handles (single definition)
GFont ui_font_title = NULL;
GFont ui_font_body = NULL;
GFont ui_font_menu = NULL;
GFont ui_font_menu_read = NULL;
GFont ui_font_caption = NULL;
GFont ui_font_menu_title = NULL;
GFont ui_font_status_bar = NULL;
GFont ui_font_splash_progress = NULL;

// This was built to handle custom fonts but is just using system fonts for now
void ui_load_fonts(void)
{
    if (!ui_font_title)
    {
        ui_font_title = fonts_get_system_font(UI_FONT_KEY_TITLE);
    }
    if (!ui_font_body)
    {
        ui_font_body = fonts_get_system_font(UI_FONT_KEY_BODY);
    }
    if (!ui_font_menu)
    {
        ui_font_menu = fonts_get_system_font(UI_FONT_KEY_MENU);
    }
    if (!ui_font_menu_read)
    {
        ui_font_menu_read = fonts_get_system_font(UI_FONT_KEY_MENU_READ);
    }
    if (!ui_font_caption)
    {
        ui_font_caption = fonts_get_system_font(UI_FONT_KEY_CAPTION);
    }
    if (!ui_font_menu_title)
    {
        ui_font_menu_title = fonts_get_system_font(UI_FONT_KEY_MENU_TITLE);
    }
    if (!ui_font_status_bar)
    {
        ui_font_status_bar = fonts_get_system_font(UI_FONT_KEY_STATUS_BAR);
    }
    if (!ui_font_splash_progress)
    {
        ui_font_splash_progress = fonts_get_system_font(UI_FONT_KEY_SPLASH_PROGRESS);
    }
}

void ui_unload_fonts(void)
{
    if (ui_font_title)
    {
        ui_font_title = NULL;
    }
    if (ui_font_body)
    {
        ui_font_body = NULL;
    }
    if (ui_font_menu)
    {
        ui_font_menu = NULL;
    }
    if (ui_font_menu_read)
    {
        ui_font_menu_read = NULL;
    }
    if (ui_font_caption)
    {
        ui_font_caption = NULL;
    }
    if (ui_font_menu_title)
    {
        ui_font_menu_title = NULL;
    }
    if (ui_font_status_bar)
    {
        ui_font_status_bar = NULL;
    }
    if (ui_font_splash_progress)
    {
        ui_font_splash_progress = NULL;
    }
}

void ui_reload_fonts(void)
{
    ui_unload_fonts();
    ui_load_fonts();
}

// 0 = follow system preferred_content_size(), 1..3 = override
static int8_t s_text_size_override = 0;

void ui_set_text_size_override(int8_t override)
{
    if (override < 0 || override > 3)
        override = 0;
    s_text_size_override = override;
}

int8_t ui_get_text_size_override(void)
{
    return s_text_size_override;
}

PreferredContentSize ui_effective_preferred_content_size(void)
{
    if (s_text_size_override == 0)
    {
        return preferred_content_size();
    }

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
    switch (s_text_size_override)
    {
    case 1:
        return PreferredContentSizeMedium;
    case 2:
        return PreferredContentSizeLarge;
    case 3:
        return PreferredContentSizeExtraLarge;
    default:
        return preferred_content_size();
    }
#else
    switch (s_text_size_override)
    {
    case 1:
        return PreferredContentSizeSmall;
    case 2:
        return PreferredContentSizeMedium;
    case 3:
        return PreferredContentSizeLarge;
    default:
        return preferred_content_size();
    }
#endif
}

UISplashProgressLayout ui_get_splash_progress_layout(GRect bounds)
{
    UISplashProgressLayout layout;

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
    const int16_t target_rect_width = 200;
    const int16_t side_padding = 16;
    const int16_t bar_height = 16;
    const int16_t label_height = 28;
    const int16_t label_gap = 32;
    const int16_t background_vertical_padding = 2;
    int16_t bar_y = ((bounds.size.h - bar_height) / 2) + 4;
#ifdef PBL_PLATFORM_EMERY
    bar_y -= 12;
#endif
#else
    const int16_t target_rect_width = 144;
    const int16_t side_padding = 12;
    const int16_t bar_height = 12;
    const int16_t label_height = 20;
    const int16_t label_gap = 24;
    const int16_t background_vertical_padding = 2;
    int16_t bar_y = ((bounds.size.h / 3) + (bar_height / 2)) + 2;
#ifdef PBL_PLATFORM_CHALK
    bar_y += 10;
#endif
#endif

    const int16_t bar_width = target_rect_width - (side_padding * 2);
    const int16_t bar_x = (bounds.size.w - bar_width) / 2;

    layout.bar_frame = GRect(bar_x, bar_y, bar_width, bar_height);
    layout.background_frame = GRect(bar_x - 2, bar_y - background_vertical_padding, bar_width + 4,
                                    bar_height + (background_vertical_padding * 2));
    layout.label_frame = GRect(bar_x, bar_y - label_gap, bar_width, label_height);
    layout.background_radius = 4;
    layout.bar_radius = 3;

    return layout;
}
