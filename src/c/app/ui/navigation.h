#pragma once

#include <pebble.h>

/**
 * Navigation Hierarchy System
 *
 * Defines a static, build-time navigation hierarchy where each screen
 * knows exactly what comes next based on tier and available data.
 *
 * Hierarchy Levels:
 * TIER_1: Categories & Story Selection
 * TIER_2: Story Summary
 * TIER_3: Available Details (decides between list/text based on data)
 * TIER_4: Detail Content (list or text)
 * TIER_5: Sources List
 * TIER_6: QR Code (terminal)
 */

typedef enum
{
    NAV_TIER_1_CATEGORIES,        // Categories list
    NAV_TIER_2_STORY_TITLES,      // Story titles list
    NAV_TIER_2_STORY_SUMMARY,     // Story summary text
    NAV_TIER_3_AVAILABLE_DETAILS, // Available details list (routing decision point)
    NAV_TIER_4_DETAIL_LIST,       // Detail options list
    NAV_TIER_4_DETAIL_TEXT,       // Detail text content
    NAV_TIER_5_SOURCES_LIST,      // Sources list
    NAV_TIER_6_QR_CODE,           // QR code display (terminal)
} NavigationTier;

typedef enum
{
    DETAIL_CONTENT_TYPE_NONE,
    DETAIL_CONTENT_TYPE_LIST,    // Has multiple options to choose from
    DETAIL_CONTENT_TYPE_TEXT,    // Has single text content
    DETAIL_CONTENT_TYPE_SOURCES, // Has sources (from text level)
} DetailContentType;

/**
 * Navigate to the next tier based on current position and available data
 *
 * @param current_tier: The current navigation tier
 * @param detail_type: The type of detail being viewed (e.g., "TalkingPoints", "Perspectives")
 * @param has_list: Whether the detail has a list of options
 * @param has_text: Whether the detail has text content
 * @param has_sources: Whether the detail has sources
 */
void navigation_proceed(NavigationTier current_tier, const char *detail_type, bool has_list, bool has_text,
                        bool has_sources);

/**
 * Navigate to story titles from categories
 * @param category: The category name
 */
void navigation_open_story_titles(const char *category);

/**
 * Navigate to a story summary from story list
 *
 * @param category: The category name
 * @param story_title: The story title
 */
void navigation_open_story_summary(const char *category, const char *story_title, int index);

/**
 * Navigate to available details from story summary
 *
 * @param category: The category name
 * @param story_id: The story ID
 */
void navigation_open_available_details(const char *category, const char *story_id);

/**
 * Navigate to detail content from available details list
 * Automatically determines whether to show list or text based on data
 *
 * @param detail_type: The type of detail (e.g., "TalkingPoints", "Perspectives")
 */
void navigation_open_detail_content(const char *detail_type);

/**
 * Navigate to detail text from detail list
 *
 * @param detail_type: The type of detail
 * @param detail_title: The specific detail option selected
 */
void navigation_open_detail_text(const char *detail_type, const char *detail_title);

/**
 * Navigate to sources list from detail text
 *
 * @param detail_type: The type of detail
 * @param detail_title: The detail title
 */
void navigation_open_sources(const char *detail_type, const char *detail_title);

/**
 * Navigate to QR code from sources list
 *
 * @param source_domain: The source domain to show QR for
 */
void navigation_open_qr_code(const char *source_domain);

/**
 * Get the current navigation tier (for debugging/logging)
 */
NavigationTier navigation_get_current_tier(void);
