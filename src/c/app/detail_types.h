#pragma once

#include "localization/localization.h"
#include <pebble.h>

typedef enum
{
    DETAIL_TYPE_PRIMARY_STORY,
    DETAIL_TYPE_INDUSTRY_IMPACT,
    DETAIL_TYPE_HUMANITARIAN_IMPACT,
    DETAIL_TYPE_TALKING_POINTS,
    DETAIL_TYPE_PERSPECTIVES,
    DETAIL_TYPE_QNA,
    DETAIL_TYPE_TRAVEL_ADVISORY,
    DETAIL_TYPE_BACKGROUND,
    DETAIL_TYPE_ARTICLES,
    DETAIL_TYPE_TECHNICAL_DETAILS,
    DETAIL_TYPE_TIMELINE,
    DETAIL_TYPE_DID_YOU_KNOW,
    DETAIL_TYPE_QUOTE,
    DETAIL_TYPE_INTERNATIONAL_REACTIONS,
    DETAIL_TYPE_USER_ACTION_ITEMS,
    DETAIL_TYPE_SCIENTIFIC_SIGNIFICANCE,
    DETAIL_TYPE_PERFORMANCE_STATISTICS,
    DETAIL_TYPE_DETAIL_SOURCES,
} DetailType;

/**
 * Configuration for each detail type
 * - detail_type: The enum value
 * - api_name: Name used in API/messaging
 * - string_id: Localization string ID for display name
 * - has_list: Does this type show a list view first?
 * - has_text: Does this type have text content?
 * - list_key: API data prefix key for list data
 */
typedef struct
{
    DetailType detail_type;
    const char *api_name;
    StringId string_id;
    bool has_list;
    bool has_text;
    const char *list_key;
} DetailTypeInfo;

/**
 * Get detail type info by API name string
 * @param api_name The API name (e.g., "TalkingPoints", "QnA")
 * @return Pointer to DetailTypeInfo, or NULL if not found
 */
const DetailTypeInfo *detail_type_from_api_name(const char *api_name);

/**
 * Get detail type info by DetailType enum
 * @param type The DetailType enum value
 * @return Pointer to DetailTypeInfo, or NULL if not found
 */
const DetailTypeInfo *detail_type_from_type(DetailType type);

/**
 * Get display name for an API name
 * Convenience function for getting just the display name
 * @param api_name The API name string
 * @return Display name string, or the api_name itself if not found
 */
const char *detail_type_get_display_name(const char *api_name);

/**
 * Get display name for a DetailType enum
 * @param type The DetailType enum value
 * @return Display name string, or "Details" if not found
 */
const char *detail_type_get_display_name_from_type(DetailType type);

/**
 * Get DetailType for an API name
 * @param api_name The API name string
 * @return DetailType enum value, or -1 if not found
 */
DetailType detail_type_get_type(const char *api_name);

/**
 * Get API name from DetailType
 * @param type The DetailType enum value
 * @return API name string, or NULL if not found
 */
const char *detail_type_get_api_name(DetailType type);

/**
 * Get the total count of registered detail types
 * @return Number of detail types
 */
int detail_type_get_count(void);

/**
 * Get detail type info by index (for iteration)
 * @param index Index from 0 to detail_type_get_count() - 1
 * @return Pointer to DetailTypeInfo, or NULL if out of bounds
 */
const DetailTypeInfo *detail_type_get_by_index(int index);
