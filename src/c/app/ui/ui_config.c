#include "ui_config.h"

// Define the font handles (single definition)
GFont ui_font_title = NULL;
GFont ui_font_body = NULL;
GFont ui_font_menu = NULL;
GFont ui_font_menu_read = NULL;
GFont ui_font_caption = NULL;
GFont ui_font_menu_title = NULL;

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

#ifdef PBL_PLATFORM_EMERY
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
