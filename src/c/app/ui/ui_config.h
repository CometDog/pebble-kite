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

/**
 * Load all UI fonts once
 */
void ui_load_fonts(void);
/**
 * Unload UI fonts
 */
void ui_unload_fonts(void);
/**
 * Reload UI fonts
 */
void ui_reload_fonts(void);

typedef struct
{
    GRect background_frame;
    GRect bar_frame;
    GRect label_frame;
    uint16_t background_radius;
    uint16_t bar_radius;
} UISplashProgressLayout;

// Colors
#ifdef PBL_COLOR
#define UI_COLOR_HIGHLIGHT GColorRajah
#define UI_COLOR_TEXT_ON_DARK GColorBlack
#ifdef PBL_ROUND
#define UI_COLOR_MENU_HEADER_BACKGROUND GColorWhite
#else
#define UI_COLOR_MENU_HEADER_BACKGROUND GColorWhite
#endif
#else
#define UI_COLOR_HIGHLIGHT GColorBlack
#define UI_COLOR_TEXT_ON_DARK GColorWhite
#define UI_COLOR_MENU_HEADER_BACKGROUND GColorWhite
#endif

#define UI_COLOR_TEXT_PRIMARY GColorBlack
#define UI_COLOR_MENU_HEADER_TEXT GColorBlack

#ifdef PBL_COLOR
#define UI_COLOR_SPLASH_PROGRESS_OUTER_BORDER GColorBlack
#define UI_COLOR_SPLASH_PROGRESS_BACKGROUND GColorBlack
#define UI_COLOR_SPLASH_PROGRESS_INNER_BORDER GColorWhite
#define UI_COLOR_SPLASH_PROGRESS_FILL GColorKellyGreen
#define UI_COLOR_SPLASH_PROGRESS_TEXT GColorBlack
#else
#define UI_COLOR_SPLASH_PROGRESS_OUTER_BORDER GColorBlack
#define UI_COLOR_SPLASH_PROGRESS_BACKGROUND GColorWhite
#define UI_COLOR_SPLASH_PROGRESS_INNER_BORDER GColorWhite
#define UI_COLOR_SPLASH_PROGRESS_FILL GColorBlack
#define UI_COLOR_SPLASH_PROGRESS_TEXT GColorBlack
#endif

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

#define UI_FONT_KEY_STATUS_BAR_SMALL FONT_KEY_GOTHIC_09
#define UI_FONT_KEY_STATUS_BAR_MEDIUM FONT_KEY_GOTHIC_14
#define UI_FONT_KEY_STATUS_BAR_LARGE FONT_KEY_GOTHIC_18
#define UI_FONT_KEY_STATUS_BAR_XLARGE FONT_KEY_GOTHIC_24

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
#define UI_FONT_KEY_SPLASH_PROGRESS FONT_KEY_GOTHIC_24_BOLD
#else
#define UI_FONT_KEY_SPLASH_PROGRESS FONT_KEY_GOTHIC_18_BOLD
#endif

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

#define UI_FONT_KEY_STATUS_BAR                                                                                         \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_STATUS_BAR_SMALL, UI_FONT_KEY_STATUS_BAR_MEDIUM, UI_FONT_KEY_STATUS_BAR_LARGE,    \
                         UI_FONT_KEY_STATUS_BAR_XLARGE)

#define UI_FONT_KEY_MENU_READ_SMALL FONT_KEY_GOTHIC_14
#define UI_FONT_KEY_MENU_READ_MEDIUM FONT_KEY_GOTHIC_18
#define UI_FONT_KEY_MENU_READ_LARGE FONT_KEY_GOTHIC_24
#define UI_FONT_KEY_MENU_READ_XLARGE FONT_KEY_GOTHIC_28

#define UI_FONT_KEY_MENU_READ                                                                                          \
    UI_FONT_KEY_FOR_SIZE(UI_FONT_KEY_MENU_READ_SMALL, UI_FONT_KEY_MENU_READ_MEDIUM, UI_FONT_KEY_MENU_READ_LARGE,       \
                         UI_FONT_KEY_MENU_READ_XLARGE)

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

// Most values taken directly from firmware source
#define UI_INDICATOR_RADIUS PBL_IF_ROUND_ELSE(12, 13)
#define UI_INDICATOR_SAFE_AREA_OFFSET 0

// Chalk has an overlap/safe area around the border because of the bezel that must be considered
#ifdef PBL_PLATFORM_CHALK
#define UI_INDICATOR_PEBBLE_ROUND_MD_OFFSET 1
#define UI_INDICATOR_PEBBLE_ROUND_LG_OFFSET -1
#else
#define UI_INDICATOR_PEBBLE_ROUND_MD_OFFSET 3
#define UI_INDICATOR_PEBBLE_ROUND_LG_OFFSET 1
#endif
#define UI_INDICATOR_X_OFFSET                                                                                          \
    (UI_INDICATOR_RADIUS + ((ui_effective_preferred_content_size() == PreferredContentSizeSmall)                       \
                                ? PBL_IF_ROUND_ELSE(UI_INDICATOR_PEBBLE_ROUND_MD_OFFSET, 8)                            \
                            : (ui_effective_preferred_content_size() == PreferredContentSizeLarge)                     \
                                ? PBL_IF_ROUND_ELSE(UI_INDICATOR_PEBBLE_ROUND_LG_OFFSET, 4)                            \
                            : (ui_effective_preferred_content_size() == PreferredContentSizeExtraLarge)                \
                                ? PBL_IF_ROUND_ELSE(UI_INDICATOR_PEBBLE_ROUND_LG_OFFSET, 4)                            \
                                : PBL_IF_ROUND_ELSE(UI_INDICATOR_PEBBLE_ROUND_MD_OFFSET, 8)))

#define UI_QR_Y_FUDGE_BASE 24
#define UI_QR_Y_FUDGE UI_SCALE_FOR_SIZE(UI_QR_Y_FUDGE_BASE)

#define UI_QR_LOADING_TEXT_HEIGHT_BASE 24
#define UI_QR_LOADING_TEXT_HEIGHT UI_SCALE_FOR_SIZE(UI_QR_LOADING_TEXT_HEIGHT_BASE)

#define UI_CUSTOM_STATUS_BAR_HEIGHT UI_SCALE_FOR_SIZE(PBL_IF_ROUND_ELSE(24, 16))

// Helper functions

// Extern font handles (defined in ui_config.c)
extern GFont ui_font_title;
extern GFont ui_font_body;
extern GFont ui_font_menu;
extern GFont ui_font_menu_read;
extern GFont ui_font_caption;
extern GFont ui_font_menu_title;
extern GFont ui_font_status_bar;
extern GFont ui_font_splash_progress;

static inline GFont ui_get_system_font_title(void)
{
    if (!ui_font_title)
    {
        ui_load_fonts();
    }
    return ui_font_title;
}

static inline GFont ui_get_system_font_body(void)
{
    if (!ui_font_body)
    {
        ui_load_fonts();
    }
    return ui_font_body;
}

static inline GFont ui_get_system_font_menu(void)
{
    if (!ui_font_menu)
    {
        ui_load_fonts();
    }
    return ui_font_menu;
}

static inline GFont ui_get_system_font_menu_read(void)
{
    if (!ui_font_menu_read)
    {
        ui_load_fonts();
    }
    return ui_font_menu_read;
}

static inline GFont ui_get_system_font_caption(void)
{
    if (!ui_font_caption)
    {
        ui_load_fonts();
    }
    return ui_font_caption;
}

static inline GFont ui_get_system_font_menu_title(void)
{
    if (!ui_font_menu_title)
    {
        ui_load_fonts();
    }
    return ui_font_menu_title;
}

static inline GFont ui_get_system_font_status_bar(void)
{
    if (!ui_font_status_bar)
    {
        ui_load_fonts();
    }
    return ui_font_status_bar;
}

static inline GFont ui_get_system_font_splash_progress(void)
{
    if (!ui_font_splash_progress)
    {
        ui_load_fonts();
    }
    return ui_font_splash_progress;
}

UISplashProgressLayout ui_get_splash_progress_layout(GRect bounds);

// Configuration
#define UI_MAX_TRACKED_WINDOWS 8
#define UI_MAX_DETAIL_CONTEXTS 4
