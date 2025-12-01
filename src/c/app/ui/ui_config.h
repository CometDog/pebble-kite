#pragma once

#include <pebble.h>
/*
 * ContentSize values:
 *   - PreferredContentSizeSmall      (not available on Emery)
 *   - PreferredContentSizeMedium     (default for most platforms)
 *   - PreferredContentSizeLarge      (default for Emery)
 *   - PreferredContentSizeExtraLarge (only available on Emery)
 */

// Colors
#ifdef PBL_COLOR
#define UI_COLOR_BACKGROUND GColorRajah
#else
#define UI_COLOR_BACKGROUND GColorWhite
#endif
#define UI_COLOR_HIGHLIGHT GColorBlack

#define UI_COLOR_TEXT_PRIMARY GColorBlack
#define UI_COLOR_TEXT_ON_DARK GColorWhite

#define UI_COLOR_MENU_HEADER_BACKGROUND GColorWhite
#define UI_COLOR_MENU_HEADER_TEXT GColorBlack

// Fonts
#define UI_FONT_KEY_TITLE_SMALL FONT_KEY_GOTHIC_14_BOLD
#define UI_FONT_KEY_TITLE_MEDIUM FONT_KEY_GOTHIC_18_BOLD
#define UI_FONT_KEY_TITLE_LARGE FONT_KEY_GOTHIC_24_BOLD
#define UI_FONT_KEY_TITLE_XLARGE FONT_KEY_GOTHIC_28_BOLD

#define UI_FONT_KEY_BODY_SMALL FONT_KEY_GOTHIC_14
#define UI_FONT_KEY_BODY_MEDIUM FONT_KEY_GOTHIC_18
#define UI_FONT_KEY_BODY_LARGE FONT_KEY_GOTHIC_24
#define UI_FONT_KEY_BODY_XLARGE FONT_KEY_GOTHIC_28

#define UI_FONT_KEY_MENU_SMALL FONT_KEY_GOTHIC_14_BOLD
#define UI_FONT_KEY_MENU_MEDIUM FONT_KEY_GOTHIC_18_BOLD
#define UI_FONT_KEY_MENU_LARGE FONT_KEY_GOTHIC_24_BOLD
#define UI_FONT_KEY_MENU_XLARGE FONT_KEY_GOTHIC_28_BOLD

#define UI_FONT_KEY_CAPTION_SMALL FONT_KEY_GOTHIC_09
#define UI_FONT_KEY_CAPTION_MEDIUM FONT_KEY_GOTHIC_14
#define UI_FONT_KEY_CAPTION_LARGE FONT_KEY_GOTHIC_18
#define UI_FONT_KEY_CAPTION_XLARGE FONT_KEY_GOTHIC_24

#define UI_FONT_KEY_MENU_TITLE_SMALL FONT_KEY_GOTHIC_09
#define UI_FONT_KEY_MENU_TITLE_MEDIUM FONT_KEY_GOTHIC_14_BOLD
#define UI_FONT_KEY_MENU_TITLE_LARGE FONT_KEY_GOTHIC_18_BOLD
#define UI_FONT_KEY_MENU_TITLE_XLARGE FONT_KEY_GOTHIC_24_BOLD

#define UI_FONT_KEY_SPLASH_LOGO_SMALL RESOURCE_ID_FONT_LUFGA_BLACK_20
#define UI_FONT_KEY_SPLASH_LOGO_MEDIUM RESOURCE_ID_FONT_LUFGA_BLACK_20
#define UI_FONT_KEY_SPLASH_LOGO_LARGE RESOURCE_ID_FONT_LUFGA_BLACK_30
#define UI_FONT_KEY_SPLASH_LOGO_XLARGE RESOURCE_ID_FONT_LUFGA_BLACK_30

#define UI_FONT_KEY_SPLASH_LOADING_SMALL RESOURCE_ID_FONT_LUFGA_REGULAR_18
#define UI_FONT_KEY_SPLASH_LOADING_MEDIUM RESOURCE_ID_FONT_LUFGA_REGULAR_18
#define UI_FONT_KEY_SPLASH_LOADING_LARGE RESOURCE_ID_FONT_LUFGA_REGULAR_28
#define UI_FONT_KEY_SPLASH_LOADING_XLARGE RESOURCE_ID_FONT_LUFGA_REGULAR_28

#define UI_FONT_KEY_FOR_SIZE(small, medium, large, xlarge)                                                             \
    ((preferred_content_size() == PreferredContentSizeSmall)        ? (small)                                          \
     : (preferred_content_size() == PreferredContentSizeLarge)      ? (large)                                          \
     : (preferred_content_size() == PreferredContentSizeExtraLarge) ? (xlarge)                                         \
                                                                    : (medium))

#define UI_FONT_KEY_TITLE                                                                                              \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_TITLE_SMALL, UI_FONT_KEY_TITLE_MEDIUM, UI_FONT_KEY_TITLE_LARGE,                   \
                         UI_FONT_KEY_TITLE_XLARGE)

#define UI_FONT_KEY_BODY                                                                                               \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_BODY_SMALL, UI_FONT_KEY_BODY_MEDIUM, UI_FONT_KEY_BODY_LARGE,                      \
                         UI_FONT_KEY_BODY_XLARGE)

#define UI_FONT_KEY_MENU                                                                                               \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_MENU_SMALL, UI_FONT_KEY_MENU_MEDIUM, UI_FONT_KEY_MENU_LARGE,                      \
                         UI_FONT_KEY_MENU_XLARGE)

#define UI_FONT_KEY_CAPTION                                                                                            \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_CAPTION_SMALL, UI_FONT_KEY_CAPTION_MEDIUM, UI_FONT_KEY_CAPTION_LARGE,             \
                         UI_FONT_KEY_CAPTION_XLARGE)

#define UI_FONT_KEY_MENU_TITLE                                                                                         \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_MENU_TITLE_SMALL, UI_FONT_KEY_MENU_TITLE_MEDIUM, UI_FONT_KEY_MENU_TITLE_LARGE,    \
                         UI_FONT_KEY_MENU_TITLE_XLARGE)

#ifdef PBL_PLATFORM_EMERY
#define UI_FONT_KEY_SPLASH_LOGO RESOURCE_ID_FONT_LUFGA_BLACK_30
#define UI_FONT_KEY_SPLASH_LOADING RESOURCE_ID_FONT_LUFGA_REGULAR_28
#else
#define UI_FONT_KEY_SPLASH_LOGO RESOURCE_ID_FONT_LUFGA_BLACK_20
#define UI_FONT_KEY_SPLASH_LOADING RESOURCE_ID_FONT_LUFGA_REGULAR_18
#endif

// Images
#ifdef PBL_PLATFORM_EMERY
#define UI_IMAGE_KAGI_NEWS_LOGO RESOURCE_ID_IMAGE_KAGI_NEWS_40
#else
#define UI_IMAGE_KAGI_NEWS_LOGO RESOURCE_ID_IMAGE_KAGI_NEWS_25
#endif

#define UI_IMAGE_MENU_ICON RESOURCE_ID_IMAGE_MENU_ICON
#define UI_IMAGE_KAGI_DOGGO RESOURCE_ID_IMAGE_KAGI_DOGGO_80

// Spacing
#define UI_MARGIN_SMALL_BASE 4
#define UI_MARGIN_MEDIUM_BASE 6
#define UI_MARGIN_LARGE_BASE 8
#define UI_MARGIN_XLARGE_BASE 12

/**
 * Scale factor based on content size.
 * Small: 0.75x, Medium: 1.0x, Large: 1.25x, XLarge: 1.5x
 */
#define UI_SCALE_FOR_SIZE(val)                                                                                         \
    ((preferred_content_size() == PreferredContentSizeSmall)        ? ((val) * 3 / 4)                                  \
     : (preferred_content_size() == PreferredContentSizeLarge)      ? ((val) * 5 / 4)                                  \
     : (preferred_content_size() == PreferredContentSizeExtraLarge) ? ((val) * 3 / 2)                                  \
                                                                    : (val))

#ifdef PBL_PLATFORM_EMERY
#define UI_SPLASH_LOGO_ICON_X_FUDGE 8
#else
#define UI_SPLASH_LOGO_ICON_X_FUDGE 4
#endif

// Layout
#define UI_MENU_X_PADDING_BASE 6
#define UI_MENU_X_PADDING UI_SCALE_FOR_SIZE(UI_MENU_X_PADDING_BASE)

#define UI_MENU_CELL_PADDING_Y_BASE 4
#define UI_MENU_CELL_PADDING_Y UI_SCALE_FOR_SIZE(UI_MENU_CELL_PADDING_Y_BASE)

#define UI_MENU_CELL_HEIGHT_BASE 40
#define UI_MENU_CELL_HEIGHT_DEFAULT UI_SCALE_FOR_SIZE(UI_MENU_CELL_HEIGHT_BASE)

#define UI_MENU_CELL_HEIGHT_THREE_LINE (UI_MENU_CELL_HEIGHT_DEFAULT * 3 / 2)
#define UI_MENU_CELL_HEIGHT_FOUR_LINE (UI_MENU_CELL_HEIGHT_DEFAULT * 2)

#define UI_LINE_HEIGHT_REGULAR_SMALL 14
#define UI_LINE_HEIGHT_REGULAR_MEDIUM 18
#define UI_LINE_HEIGHT_REGULAR_LARGE 24
#define UI_LINE_HEIGHT_REGULAR_XLARGE 28

#define UI_LINE_HEIGHT_REGULAR                                                                                         \
    ((preferred_content_size() == PreferredContentSizeSmall)        ? UI_LINE_HEIGHT_REGULAR_SMALL                     \
     : (preferred_content_size() == PreferredContentSizeLarge)      ? UI_LINE_HEIGHT_REGULAR_LARGE                     \
     : (preferred_content_size() == PreferredContentSizeExtraLarge) ? UI_LINE_HEIGHT_REGULAR_XLARGE                    \
                                                                    : UI_LINE_HEIGHT_REGULAR_MEDIUM)

#define UI_DETAIL_SIDE_PADDING_BASE 4
#define UI_DETAIL_SIDE_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_SIDE_PADDING_BASE)

#define UI_DETAIL_BETWEEN_PADDING_BASE 6
#define UI_DETAIL_BETWEEN_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_BETWEEN_PADDING_BASE)

#define UI_DETAIL_BOTTOM_PADDING_BASE 8
#define UI_DETAIL_BOTTOM_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_BOTTOM_PADDING_BASE)

#define UI_TEXT_SAFETY_PIXELS 2

#define UI_INDICATOR_RADIUS 10

#define UI_SPLASH_LOGO_TOP_MARGIN_BASE 15
#define UI_SPLASH_LOGO_TOP_MARGIN UI_SCALE_FOR_SIZE(UI_SPLASH_LOGO_TOP_MARGIN_BASE)

#define UI_SPLASH_LOGO_HEIGHT_BASE 24
#define UI_SPLASH_LOGO_HEIGHT UI_SCALE_FOR_SIZE(UI_SPLASH_LOGO_HEIGHT_BASE)

#define UI_SPLASH_LOADING_HEIGHT_BASE 22
#define UI_SPLASH_LOADING_HEIGHT UI_SCALE_FOR_SIZE(UI_SPLASH_LOADING_HEIGHT_BASE)

#define UI_SPLASH_LOADING_Y_OFFSET_BASE 30
#define UI_SPLASH_LOADING_Y_OFFSET UI_SCALE_FOR_SIZE(UI_SPLASH_LOADING_Y_OFFSET_BASE)

#define UI_QR_Y_FUDGE_BASE 24
#define UI_QR_Y_FUDGE UI_SCALE_FOR_SIZE(UI_QR_Y_FUDGE_BASE)

#define UI_QR_LOADING_TEXT_HEIGHT_BASE 24
#define UI_QR_LOADING_TEXT_HEIGHT UI_SCALE_FOR_SIZE(UI_QR_LOADING_TEXT_HEIGHT_BASE)

// Helper functions
/**
 * Get system font for the current content size preference.
 *
 * @param font_type One of: UI_FONT_TYPE_TITLE, UI_FONT_TYPE_BODY, etc.
 * @return GFont from system fonts
 */
static inline GFont ui_get_system_font_title(void)
{
    return fonts_get_system_font(UI_FONT_KEY_TITLE);
}

static inline GFont ui_get_system_font_body(void)
{
    return fonts_get_system_font(UI_FONT_KEY_BODY);
}

static inline GFont ui_get_system_font_menu(void)
{
    return fonts_get_system_font(UI_FONT_KEY_MENU);
}

static inline GFont ui_get_system_font_caption(void)
{
    return fonts_get_system_font(UI_FONT_KEY_CAPTION);
}

static inline GFont ui_get_system_font_menu_title(void)
{
    return fonts_get_system_font(UI_FONT_KEY_MENU_TITLE);
}

static inline ResHandle ui_get_splash_logo_font_handle(void)
{
    return resource_get_handle(UI_FONT_KEY_SPLASH_LOGO);
}

static inline ResHandle ui_get_splash_loading_font_handle(void)
{
    return resource_get_handle(UI_FONT_KEY_SPLASH_LOADING);
}

// Configuration
#define UI_MAX_TRACKED_WINDOWS 16
#define UI_MAX_DETAIL_CONTEXTS 8
