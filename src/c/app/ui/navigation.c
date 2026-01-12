#include "navigation.h"
#include "../communication.h"
#include "../data.h"
#include "../detail_types.h"
#include "../utils/debug_logger.h"
#include "./categories.h"
#include "./detail_window_registry.h"
#include "./main_story/main_story_list_window.h"
#include "./main_story/main_story_text_window.h"
#include "./qr_view.h"
#include "./source/active_detail_source_list_window.h"
#include "./source/source_detail_list_window.h"
#include "./story_details.h"
#include <string.h>

static NavigationTier s_current_tier = NAV_TIER_1_CATEGORIES;

NavigationTier navigation_get_current_tier(void)
{
    return s_current_tier;
}

void navigation_open_story_titles(const char *category, int category_index)
{
    s_current_tier = NAV_TIER_2_STORY_TITLES;

    clear_story_list_data();

    set_selected_category((char *)category);
    set_selected_category_index(category_index);
    send_get_story_titles(1, (char *)category, true);
    main_story_list_window_ui_init();
}

void navigation_open_story_summary(const char *category, const char *story_title, int index)
{
    s_current_tier = NAV_TIER_2_STORY_SUMMARY;

    clear_story_data();

    send_get_short_summary((char *)category, (char *)story_title);
    set_story_full_title((char *)story_title);
    main_story_text_window_ui_init(index);
}

void navigation_open_available_details(const char *category, const char *story_id)
{
    s_current_tier = NAV_TIER_3_AVAILABLE_DETAILS;

    clear_available_details_data();

    send_get_story_available_details((char *)category, (char *)story_id);
    story_details_ui_init();
}

void navigation_open_detail_content(const char *detail_type)
{
    clear_detail_list_data();
    clear_detail_text_data();

    set_detail_type((char *)detail_type, NULL);
    send_get_story_detail((char *)detail_type);

    const DetailTypeInfo *info = detail_type_from_api_name(detail_type);
    if (!info)
    {
        ERROR_LOG("KNCNavigation", "Unknown detail type: %s", detail_type);
        return;
    }

    if (info->detail_type == DETAIL_TYPE_ARTICLES)
    {
        s_current_tier = NAV_TIER_5_SOURCES_LIST;
        source_detail_list_window_ui_init();
    }
    else if (info->has_list)
    {
        s_current_tier = NAV_TIER_4_DETAIL_LIST;
        detail_window_registry_init_list(info->detail_type);
    }
    else if (info->has_text)
    {
        s_current_tier = NAV_TIER_4_DETAIL_TEXT;
        detail_window_registry_init_text(info->detail_type);
    }
}

void navigation_open_detail_text(const char *detail_type, const char *detail_title)
{
    s_current_tier = NAV_TIER_4_DETAIL_TEXT;

    // The detail_type should already be set, just set the specific title
    set_detail_title((char *)detail_title, NULL);
    send_get_current_detail_text((char *)detail_type, (char *)detail_title);

    const DetailTypeInfo *info = detail_type_from_api_name(detail_type);
    if (!info)
    {
        ERROR_LOG("KNCNavigation", "Unknown detail type for text: %s", detail_type);
        return;
    }

    detail_window_registry_init_text(info->detail_type);
}

void navigation_open_sources(const char *detail_type, const char *detail_title)
{
    s_current_tier = NAV_TIER_5_SOURCES_LIST;

    send_get_story_detail_sources((char *)detail_type, (char *)detail_title);
    active_detail_source_list_window_ui_init();
}

void navigation_open_qr_code(const char *source_domain)
{
    s_current_tier = NAV_TIER_6_QR_CODE;

    send_get_qr_code_bitmap((char *)source_domain);
    qr_view_init();
}

void navigation_proceed(NavigationTier current_tier, const char *detail_type, bool has_list, bool has_text,
                        bool has_sources)
{
    DEBUG_LOG("KNCNavigation", "navigation_proceed from tier=%d, detail_type=%s, list=%d, text=%d, sources=%d",
              current_tier, detail_type ? detail_type : "NULL", has_list, has_text, has_sources);

    // This function can be used for dynamic routing based on data availability
    // Currently not used but kept for future extensibility
}
