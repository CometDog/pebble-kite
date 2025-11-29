#include "navigation.h"
#include "../communication.h"
#include "../data.h"
#include "../detail_types.h"
#include "./action_items/action_items_detail_list_window.h"
#include "./action_items/action_items_detail_text_window.h"
#include "./background/background_detail_text_window.h"
#include "./categories.h"
#include "./did_you_know/did_you_know_detail_text_window.h"
#include "./humanitarian_impact/humanitarian_impact_detail_text_window.h"
#include "./industry_impact/industry_impact_detail_list_window.h"
#include "./industry_impact/industry_impact_detail_text_window.h"
#include "./international_reactions/international_reactions_detail_list_window.h"
#include "./international_reactions/international_reactions_detail_text_window.h"
#include "./main_story/main_story_list_window.h"
#include "./main_story/main_story_text_window.h"
#include "./performance_statistics/performance_statistics_detail_list_window.h"
#include "./performance_statistics/performance_statistics_detail_text_window.h"
#include "./perspective/perspective_detail_list_window.h"
#include "./perspective/perspective_detail_text_window.h"
#include "./qna/qna_detail_list_window.h"
#include "./qna/qna_detail_text_window.h"
#include "./qr_view.h"
#include "./quote/quote_detail_text_window.h"
#include "./scientific_significance/scientific_significance_detail_list_window.h"
#include "./scientific_significance/scientific_significance_detail_text_window.h"
#include "./source/active_detail_source_list_window.h"
#include "./source/source_detail_list_window.h"
#include "./story_details.h"
#include "./talking_point/talking_point_detail_list_window.h"
#include "./talking_point/talking_point_detail_text_window.h"
#include "./technical_detail/technical_detail_detail_list_window.h"
#include "./technical_detail/technical_detail_detail_text_window.h"
#include "./timeline/timeline_detail_list_window.h"
#include "./timeline/timeline_detail_text_window.h"
#include "./travel_advisory/travel_advisory_detail_list_window.h"
#include "./travel_advisory/travel_advisory_detail_text_window.h"
#include <string.h>

static NavigationTier s_current_tier = NAV_TIER_1_CATEGORIES;

typedef struct
{
    DetailType detail_type;
    void (*list_init)(void); // NULL if type goes directly to text
    void (*text_init)(void); // NULL if type has no text view
} DetailNavRoute;

static const DetailNavRoute s_detail_routes[] = {
    // Text-only types (no list view)
    {DETAIL_TYPE_BACKGROUND, NULL, background_detail_text_window_ui_init},
    {DETAIL_TYPE_HUMANITARIAN_IMPACT, NULL, humanitarian_impact_detail_text_window_ui_init},
    {DETAIL_TYPE_DID_YOU_KNOW, NULL, did_you_know_detail_text_window_ui_init},
    {DETAIL_TYPE_QUOTE, NULL, quote_detail_text_window_ui_init},

    // List + Text types
    {DETAIL_TYPE_TALKING_POINTS, talking_point_detail_list_window_ui_init, talking_point_detail_text_window_ui_init},
    {DETAIL_TYPE_PERSPECTIVES, perspective_detail_list_window_ui_init, perspective_detail_text_window_ui_init},
    {DETAIL_TYPE_QNA, qna_detail_list_window_ui_init, qna_detail_text_window_ui_init},
    {DETAIL_TYPE_TECHNICAL_DETAILS, technical_detail_detail_list_window_ui_init,
     technical_detail_detail_text_window_ui_init},
    {DETAIL_TYPE_INDUSTRY_IMPACT, industry_impact_detail_list_window_ui_init,
     industry_impact_detail_text_window_ui_init},
    {DETAIL_TYPE_TIMELINE, timeline_detail_list_window_ui_init, timeline_detail_text_window_ui_init},
    {DETAIL_TYPE_TRAVEL_ADVISORY, travel_advisory_detail_list_window_ui_init,
     travel_advisory_detail_text_window_ui_init},
    {DETAIL_TYPE_INTERNATIONAL_REACTIONS, international_reactions_detail_list_window_ui_init,
     international_reactions_detail_text_window_ui_init},
    {DETAIL_TYPE_USER_ACTION_ITEMS, action_items_detail_list_window_ui_init, action_items_detail_text_window_ui_init},
    {DETAIL_TYPE_PERFORMANCE_STATISTICS, performance_statistics_detail_list_window_ui_init,
     performance_statistics_detail_text_window_ui_init},
    {DETAIL_TYPE_SCIENTIFIC_SIGNIFICANCE, scientific_significance_detail_list_window_ui_init,
     scientific_significance_detail_text_window_ui_init},

    // List-only type (sources)
    {DETAIL_TYPE_ARTICLES, source_detail_list_window_ui_init, NULL},
};

static const int s_detail_routes_count = sizeof(s_detail_routes) / sizeof(s_detail_routes[0]);

static const DetailNavRoute *find_route_for_type(DetailType type)
{
    for (int i = 0; i < s_detail_routes_count; i++)
    {
        if (s_detail_routes[i].detail_type == type)
        {
            return &s_detail_routes[i];
        }
    }
    return NULL;
}

NavigationTier navigation_get_current_tier(void)
{
    return s_current_tier;
}

void navigation_open_story_titles(const char *category)
{
    s_current_tier = NAV_TIER_2_STORY_TITLES;

    clear_story_list_data();

    set_selected_category((char *)category);
    send_get_story_titles(1, (char *)category, true);
    main_story_list_window_ui_init();
}

void navigation_open_story_summary(const char *category, const char *story_title)
{
    s_current_tier = NAV_TIER_2_STORY_SUMMARY;

    clear_story_data();

    send_get_short_summary((char *)category, (char *)story_title);
    set_story_full_title((char *)story_title);
    main_story_text_window_ui_init();
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
        APP_LOG(APP_LOG_LEVEL_ERROR, "[KNCNavigation] Unknown detail type: %s", detail_type);
        return;
    }

    const DetailNavRoute *route = find_route_for_type(info->detail_type);
    if (!route)
    {
        APP_LOG(APP_LOG_LEVEL_ERROR, "[KNCNavigation] No route for detail type: %s", detail_type);
        return;
    }

    if (info->has_list && route->list_init)
    {
        s_current_tier = (info->detail_type == DETAIL_TYPE_ARTICLES) ? NAV_TIER_5_SOURCES_LIST : NAV_TIER_4_DETAIL_LIST;
        route->list_init();
    }
    else if (route->text_init)
    {
        s_current_tier = NAV_TIER_4_DETAIL_TEXT;
        route->text_init();
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
        APP_LOG(APP_LOG_LEVEL_ERROR, "[KNCNavigation] Unknown detail type for text: %s", detail_type);
        return;
    }

    const DetailNavRoute *route = find_route_for_type(info->detail_type);
    if (route && route->text_init)
    {
        route->text_init();
    }
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
    APP_LOG(APP_LOG_LEVEL_DEBUG,
            "[KNCNavigation] navigation_proceed from tier=%d, detail_type=%s, list=%d, text=%d, sources=%d",
            current_tier, detail_type ? detail_type : "NULL", has_list, has_text, has_sources);

    // This function can be used for dynamic routing based on data availability
    // Currently not used but kept for future extensibility
}
