#include "detail_types.h"
#include <string.h>

static const DetailTypeInfo s_detail_types[] = {
    // Text-only types (no list view)
    {DETAIL_TYPE_BACKGROUND, "Background", "Background", false, true, NULL},
    {DETAIL_TYPE_HUMANITARIAN_IMPACT, "HumanitarianImpact", "Human Impact", false, true, NULL},
    {DETAIL_TYPE_DID_YOU_KNOW, "DidYouKnow", "Did You Know?", false, true, NULL},
    {DETAIL_TYPE_QUOTE, "Quote", "Quote", false, true, NULL},

    // List + Text types
    {DETAIL_TYPE_TALKING_POINTS, "TalkingPoints", "Highlights", true, true, "povs"},
    {DETAIL_TYPE_PERSPECTIVES, "Perspectives", "Perspectives", true, true, "povs"},
    {DETAIL_TYPE_QNA, "QnA", "Quick Questions", true, true, "qnas"},
    {DETAIL_TYPE_TECHNICAL_DETAILS, "TechnicalDetails", "Technical Details", true, true, "povs"},
    {DETAIL_TYPE_INDUSTRY_IMPACT, "IndustryImpact", "Industry Impact", true, true, "povs"},
    {DETAIL_TYPE_TIMELINE, "Timeline", "Timeline", true, true, "dates"},
    {DETAIL_TYPE_TRAVEL_ADVISORY, "TravelAdvisory", "Travel Advisory", true, true, "advisories"},
    {DETAIL_TYPE_INTERNATIONAL_REACTIONS, "InternationalReactions", "Int'l Reactions", true, true, "povs"},
    {DETAIL_TYPE_USER_ACTION_ITEMS, "UserActionItems", "Action Items", true, true, "povs"},
    {DETAIL_TYPE_SCIENTIFIC_SIGNIFICANCE, "ScientificSignificance", "Significance", true, true, "povs"},
    {DETAIL_TYPE_PERFORMANCE_STATISTICS, "PerformanceStatistics", "Performance", true, true, "povs"},

    // List-only type (sources)
    {DETAIL_TYPE_ARTICLES, "Articles", "Sources", true, false, NULL},
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
    return info ? info->display_name : api_name;
}

const char *detail_type_get_display_name_from_type(DetailType type)
{
    const DetailTypeInfo *info = detail_type_from_type(type);
    return info ? info->display_name : "Details";
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
