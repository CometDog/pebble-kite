#include "detail_types.h"
#include "localization/localization.h"
#include <string.h>

static const DetailTypeInfo s_detail_types[] = {
    // Text-only types (no list view)
    {DETAIL_TYPE_BACKGROUND, "Background", STRING_HISTORICAL_BACKGROUND, false, true, NULL},
    {DETAIL_TYPE_HUMANITARIAN_IMPACT, "HumanitarianImpact", STRING_HUMANITARIAN_IMPACT, false, true, NULL},
    {DETAIL_TYPE_DID_YOU_KNOW, "DidYouKnow", STRING_DID_YOU_KNOW, false, true, NULL},
    {DETAIL_TYPE_QUOTE, "Quote", STRING_QUOTE, false, true, NULL},

    // List + Text types
    {DETAIL_TYPE_TALKING_POINTS, "TalkingPoints", STRING_HIGHLIGHTS, true, true, "povs"},
    {DETAIL_TYPE_PERSPECTIVES, "Perspectives", STRING_PERSPECTIVES, true, true, "povs"},
    {DETAIL_TYPE_QNA, "QnA", STRING_QUICK_QUESTIONS, true, true, "qnas"},
    {DETAIL_TYPE_TECHNICAL_DETAILS, "TechnicalDetails", STRING_TECHNICAL_DETAILS, true, true, "povs"},
    {DETAIL_TYPE_INDUSTRY_IMPACT, "IndustryImpact", STRING_INDUSTRY_IMPACT, true, true, "povs"},
    {DETAIL_TYPE_TIMELINE, "Timeline", STRING_TIMELINE, true, true, "dates"},
    {DETAIL_TYPE_TRAVEL_ADVISORY, "TravelAdvisory", STRING_TRAVEL_ADVISORY, true, true, "advisories"},
    {DETAIL_TYPE_INTERNATIONAL_REACTIONS, "InternationalReactions", STRING_INTERNATIONAL_REACTIONS, true, true, "povs"},
    {DETAIL_TYPE_USER_ACTION_ITEMS, "UserActionItems", STRING_ACTION_ITEMS, true, true, "povs"},
    {DETAIL_TYPE_SCIENTIFIC_SIGNIFICANCE, "ScientificSignificance", STRING_SCIENTIFIC_SIGNIFICANCE, true, true, "povs"},
    {DETAIL_TYPE_PERFORMANCE_STATISTICS, "PerformanceStatistics", STRING_PERFORMANCE_STATISTICS, true, true, "povs"},

    // List-only type (sources)
    {DETAIL_TYPE_ARTICLES, "Articles", STRING_SOURCES, true, false, NULL},
};

static const int s_detail_types_count = sizeof(s_detail_types) / sizeof(s_detail_types[0]);

const DetailTypeInfo *detail_type_from_api_name(const char *api_name)
{
    if (!api_name)
    {
        return NULL;
    }

    for (int i = 0; i < s_detail_types_count; i++)
    {
        if (strcmp(s_detail_types[i].api_name, api_name) == 0)
        {
            return &s_detail_types[i];
        }
    }
    return NULL;
}

const DetailTypeInfo *detail_type_from_type(DetailType type)
{
    for (int i = 0; i < s_detail_types_count; i++)
    {
        if (s_detail_types[i].detail_type == type)
        {
            return &s_detail_types[i];
        }
    }
    return NULL;
}

const char *detail_type_get_display_name(const char *api_name)
{
    const DetailTypeInfo *info = detail_type_from_api_name(api_name);
    return info ? localization_get_string(info->string_id) : api_name;
}

const char *detail_type_get_display_name_from_type(DetailType type)
{
    const DetailTypeInfo *info = detail_type_from_type(type);
    return info ? localization_get_string(info->string_id) : "Details";
}

DetailType detail_type_get_type(const char *api_name)
{
    const DetailTypeInfo *info = detail_type_from_api_name(api_name);
    return info ? info->detail_type : (DetailType)-1;
}

const char *detail_type_get_api_name(DetailType type)
{
    const DetailTypeInfo *info = detail_type_from_type(type);
    return info ? info->api_name : NULL;
}

int detail_type_get_count(void)
{
    return s_detail_types_count;
}

const DetailTypeInfo *detail_type_get_by_index(int index)
{
    if (index < 0 || index >= s_detail_types_count)
    {
        return NULL;
    }
    return &s_detail_types[index];
}
