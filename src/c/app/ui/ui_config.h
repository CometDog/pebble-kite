#pragma once

#include <pebble.h>
/*
 * ContentSize values:
 *   - PreferredContentSizeSmall      (not available on Emery)
 *   - PreferredContentSizeMedium     (default for most platforms)
 *   - PreferredContentSizeLarge      (default for Emery)
 *   - PreferredContentSizeExtraLarge (only available on Emery)
 */

/**
 * Set text size override
 * @param override: 0 = follow system preferred_content_size(), 1 = Small, 2 = Medium, 3 = Large
 */
void ui_set_text_size_override(int8_t override);
/**
 * Get text size override
 * @return override value: 0 = follow system preferred_content_size(), 1 = Small, 2 = Medium, 3 = Large
 */
int8_t ui_get_text_size_override(void);
/**
 * Get effective preferred content size considering override
 * @return PreferredContentSize value
 */
PreferredContentSize ui_effective_preferred_content_size(void);

// Colors
#ifdef PBL_COLOR
#define UI_COLOR_HIGHLIGHT GColorVividCerulean
#define UI_COLOR_TEXT_ON_DARK GColorBlack
#else
#define UI_COLOR_HIGHLIGHT GColorBlack
#define UI_COLOR_TEXT_ON_DARK GColorWhite
#endif

#define UI_COLOR_TEXT_PRIMARY GColorBlack

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

#define UI_FONT_KEY_FOR_SIZE(small, medium, large, xlarge)                                                             \
    ((ui_effective_preferred_content_size() == PreferredContentSizeSmall)        ? (small)                             \
     : (ui_effective_preferred_content_size() == PreferredContentSizeLarge)      ? (large)                             \
     : (ui_effective_preferred_content_size() == PreferredContentSizeExtraLarge) ? (xlarge)                            \
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

// Images
#define UI_IMAGE_MENU_ICON RESOURCE_ID_IMAGE_MENU_ICON

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
    ((ui_effective_preferred_content_size() == PreferredContentSizeSmall)        ? ((val) * 3 / 4)                     \
     : (ui_effective_preferred_content_size() == PreferredContentSizeLarge)      ? ((val) * 5 / 4)                     \
     : (ui_effective_preferred_content_size() == PreferredContentSizeExtraLarge) ? ((val) * 3 / 2)                     \
                                                                                 : (val))

// Layout
#ifdef PBL_ROUND
#define UI_MENU_TEXT_ALIGNMENT GTextAlignmentCenter
#else
#define UI_MENU_TEXT_ALIGNMENT GTextAlignmentLeft
#endif

#ifdef PBL_ROUND
#define UI_MENU_X_PADDING_BASE 10
#define UI_MENU_HEADER_X_PADDING_BASE 0
#else
#define UI_MENU_X_PADDING_BASE 6
#define UI_MENU_HEADER_X_PADDING_BASE 3
#endif
#define UI_MENU_CELL_PADDING_Y_BASE 4

#define UI_MENU_HEADER_X_PADDING UI_SCALE_FOR_SIZE(UI_MENU_HEADER_X_PADDING_BASE)
#define UI_MENU_X_PADDING UI_SCALE_FOR_SIZE(UI_MENU_X_PADDING_BASE)
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
    ((ui_effective_preferred_content_size() == PreferredContentSizeSmall)        ? UI_LINE_HEIGHT_REGULAR_SMALL        \
     : (ui_effective_preferred_content_size() == PreferredContentSizeLarge)      ? UI_LINE_HEIGHT_REGULAR_LARGE        \
     : (ui_effective_preferred_content_size() == PreferredContentSizeExtraLarge) ? UI_LINE_HEIGHT_REGULAR_XLARGE       \
                                                                                 : UI_LINE_HEIGHT_REGULAR_MEDIUM)

#define UI_DETAIL_SIDE_PADDING_BASE 4
#define UI_DETAIL_SIDE_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_SIDE_PADDING_BASE)

#ifdef PBL_ROUND
#define UI_DETAIL_SIDE_PADDING_EXTRA UI_SCALE_FOR_SIZE(12)
#else
#define UI_DETAIL_SIDE_PADDING_EXTRA 0
#endif

#ifdef PBL_ROUND
#define UI_DETAIL_TOP_PADDING_FOR_ROUND(h) ((h) / 2)
#define UI_DETAIL_BOTTOM_PADDING_FOR_ROUND(h) ((h) / 2)
#else
#define UI_DETAIL_TOP_PADDING_FOR_ROUND(h) (0)
#define UI_DETAIL_BOTTOM_PADDING_FOR_ROUND(h) (0)
#endif

#define UI_DETAIL_TITLE_Y_INITIAL(title_h, side_padding, scroll_h)                                                     \
    (UI_DETAIL_TOP_PADDING_FOR_ROUND(scroll_h)                                                                         \
         ? ((UI_DETAIL_TOP_PADDING_FOR_ROUND(scroll_h) - ((title_h) / 2)) < (side_padding)                             \
                ? (side_padding)                                                                                       \
                : (UI_DETAIL_TOP_PADDING_FOR_ROUND(scroll_h) - ((title_h) / 2)))                                       \
         : (side_padding))
#ifdef PBL_ROUND
#define UI_DETAIL_TEXT_ALIGNMENT GTextAlignmentCenter
#else
#define UI_DETAIL_TEXT_ALIGNMENT GTextAlignmentLeft
#endif

#define UI_DETAIL_BETWEEN_PADDING_BASE 6
#define UI_DETAIL_BETWEEN_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_BETWEEN_PADDING_BASE)

#define UI_DETAIL_BOTTOM_PADDING_BASE 8
#define UI_DETAIL_BOTTOM_PADDING UI_SCALE_FOR_SIZE(UI_DETAIL_BOTTOM_PADDING_BASE)

#define UI_TEXT_SAFETY_PIXELS 2

#define UI_INDICATOR_RADIUS 10
#ifdef PBL_ROUND
#define UI_INDICATOR_X_OFFSET 3
#else
#define UI_INDICATOR_X_OFFSET 5
#endif

#define UI_QR_Y_FUDGE_BASE 24
#define UI_QR_Y_FUDGE UI_SCALE_FOR_SIZE(UI_QR_Y_FUDGE_BASE)

#define UI_QR_LOADING_TEXT_HEIGHT_BASE 24
#define UI_QR_LOADING_TEXT_HEIGHT UI_SCALE_FOR_SIZE(UI_QR_LOADING_TEXT_HEIGHT_BASE)

// Helper functions

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

// Configuration
#define UI_MAX_TRACKED_WINDOWS 16
#define UI_MAX_DETAIL_CONTEXTS 8
