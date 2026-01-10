#include "data.h"
#include "localization/localization.h"
#include "ui/background/background_detail_text_window.h"
#include "ui/categories.h"
#include "ui/humanitarian_impact/humanitarian_impact_detail_text_window.h"
#include "ui/industry_impact/industry_impact_detail_list_window.h"
#include "ui/industry_impact/industry_impact_detail_text_window.h"
#include "ui/main_story/main_story_list_window.h"
#include "ui/main_story/main_story_text_window.h"
#include "ui/perspective/perspective_detail_text_window.h"
#include "ui/qna/qna_detail_list_window.h"
#include "ui/qna/qna_detail_text_window.h"
#include "ui/qr_view.h"
#include "ui/quote/quote_detail_text_window.h"
#include "ui/source/source_detail_list_window.h"
#include "ui/splash_screen.h"
#include "ui/story_details.h"
#include "ui/talking_point/talking_point_detail_list_window.h"
#include "ui/talking_point/talking_point_detail_text_window.h"
#include "ui/technical_detail/technical_detail_detail_list_window.h"
#include "ui/technical_detail/technical_detail_detail_text_window.h"
#include "ui/timeline/timeline_detail_list_window.h"
#include "ui/timeline/timeline_detail_text_window.h"
#include "ui/travel_advisory/travel_advisory_detail_text_window.h"
#include "utils/debug_logger.h"
#include "utils/string_utils.h"

#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS_HIGH 100
#define MAX_ITEMS_LOW 20
#define QR_MAX_CAPACITY 512

/**
 * HIERARCHICAL DATA STRUCTURE
 *
 * This module organizes data into 4 levels that mirror the UI navigation hierarchy.
 * Each level retains its data independently, allowing navigation forward and backward
 * through the UI while preserving context at each level.
 *
 * Navigation Flow:
 *
 *   Level 1: Categories
 *            ↓ (select category)
 *   Level 2: Story List + Story Text (list of stories + selected story summary)
 *            ↓ (select detail type)
 *   Level 3: Available Details List (types of details available for the story)
 *            ↓ (select detail option)
 *   Level 4: Selected Detail (detail list/options + detail text + sources)
 *
 * Data Management:
 * - When navigating FORWARD (deeper), load new data into the next level
 * - When navigating BACKWARD, call clear_level_N() to clear that level and all deeper levels
 * - Each level preserves its data until explicitly cleared
 * - Use the helper functions clear_level_N() for proper hierarchical cleanup
 *
 * Example Usage:
 *
 *   // User navigates: Categories → Story View → Available Details → Detail Text
 *   // Each level loads its data as the user progresses
 *
 *   // User presses back from Detail Text to Available Details:
 *   clear_level_4_selected_detail();  // Clears only level 4
 *
 *   // User presses back from Available Details to Story View:
 *   clear_level_3_available_details(); // Clears levels 3 and 4
 *
 *   // User presses back from Story View to Categories:
 *   clear_level_2_story_view();       // Clears levels 2, 3, and 4
 */

// Navigation hierarchy levels - matches UI stack
typedef struct
{
    char *categories[MAX_ITEMS_HIGH];
    uint16_t categories_count;
    uint8_t additional_feeds_count;
} CategoriesLevel;

typedef struct
{
    char *selected_category;
    char *story_titles[MAX_ITEMS_LOW];
    bool stories_read[MAX_ITEMS_LOW];
    uint16_t story_titles_count;
    char *story_id;
    char *story_full_title;
    char *story_short_summary;
} StoryViewLevel;

typedef struct
{
    char *available_details[MAX_ITEMS_LOW];
    uint16_t available_details_count;
} AvailableDetailsLevel;

typedef struct
{
    char *detail_type;
    char *detail_options[MAX_ITEMS_LOW];
    uint16_t detail_options_count;
    char *detail_title;
    char *detail_text;
    bool has_sources;
    char *sources[MAX_ITEMS_LOW];
    uint16_t sources_count;
    uint8_t *qr_code_chunks;
    uint16_t qr_code_chunks_capacity;
    uint16_t qr_code_chunks_count;
    uint8_t qr_size;
} SelectedDetailLevel;

static CategoriesLevel level_1_categories = {{0}, 0, 0};
static StoryViewLevel level_2_story_view = {NULL, {0}, {0}, 0, NULL, NULL, NULL};
static AvailableDetailsLevel level_3_available_details = {{0}, 0};
static SelectedDetailLevel level_4_selected_detail = {0};

// ============================================================================
// Level 1: Categories Implementation
// ============================================================================

void set_additional_feeds_count(uint8_t count)
{
    level_1_categories.additional_feeds_count = count;
}

uint8_t get_additional_feeds_count(void)
{
    return level_1_categories.additional_feeds_count;
}

void push_local_categories(char *categories_string)
{
    bool was_empty = (level_1_categories.categories_count == 0);
    level_1_categories.categories_count += string_split_to_array(
        level_1_categories.categories, level_1_categories.categories_count, MAX_ITEMS_HIGH, categories_string, "||");

    // Dismiss splash screen on first category load
    if (was_empty && level_1_categories.categories_count > 0 && splash_screen_is_showing())
    {
        splash_screen_dismiss();
    }
    else
    {
        request_categories_menu_update();
    }
}

const CategoriesData *get_categories_data(void)
{
    static CategoriesData data;
    if (level_1_categories.categories_count == 0)
    {
        static const char *loading_arr[1];
        loading_arr[0] = localization_get_string(STRING_LOADING);
        data.categories = (char **)loading_arr;
        data.categories_count = 1;
    }
    else
    {
        data.categories = level_1_categories.categories;
        data.categories_count = level_1_categories.categories_count;
    }
    return &data;
}

bool categories_loaded(void)
{
    return level_1_categories.categories_count > 0;
}

void clear_categories_data(void)
{
    for (int i = 0; i < level_1_categories.categories_count; ++i)
    {
        if (level_1_categories.categories[i])
        {
            free(level_1_categories.categories[i]);
            level_1_categories.categories[i] = NULL;
        }
    }
    level_1_categories.categories_count = 0;
}

// ============================================================================
// Level 2: Story List + Story Data Implementation
// ============================================================================

void set_selected_category(char *category_name)
{
    if (level_2_story_view.selected_category)
    {
        free(level_2_story_view.selected_category);
        level_2_story_view.selected_category = NULL;
    }
    if (category_name)
    {
        level_2_story_view.selected_category = string_duplicate(category_name);
    }
}

void push_story_titles(char *story_titles_string)
{
    level_2_story_view.story_titles_count +=
        string_split_to_array(level_2_story_view.story_titles, level_2_story_view.story_titles_count, MAX_ITEMS_LOW,
                              story_titles_string, "||");
    main_story_list_window_ui_update();
}

const StoryListData *get_story_list_data(void)
{
    static StoryListData data;
    data.selected_category = level_2_story_view.selected_category;
    if (level_2_story_view.story_titles_count == 0)
    {
        static const char *loading_arr[1];
        loading_arr[0] = localization_get_string(STRING_LOADING);
        static bool loading_read_arr[1] = {false};
        data.story_titles = (char **)loading_arr;
        data.stories_read = loading_read_arr;
        data.story_titles_count = 1;
    }
    else
    {
        data.story_titles = level_2_story_view.story_titles;
        data.stories_read = level_2_story_view.stories_read;
        data.story_titles_count = level_2_story_view.story_titles_count;
    }
    return &data;
}

void clear_story_list_data(void)
{
    if (level_2_story_view.selected_category)
    {
        free(level_2_story_view.selected_category);
        level_2_story_view.selected_category = NULL;
    }
    for (int i = 0; i < level_2_story_view.story_titles_count; ++i)
    {
        if (level_2_story_view.story_titles[i])
        {
            free(level_2_story_view.story_titles[i]);
            level_2_story_view.story_titles[i] = NULL;
        }
    }
    level_2_story_view.story_titles_count = 0;
    main_story_list_window_ui_update();
}

void set_story_id(char *story_id)
{
    if (level_2_story_view.story_id)
    {
        free(level_2_story_view.story_id);
        level_2_story_view.story_id = NULL;
    }
    if (story_id)
    {
        level_2_story_view.story_id = string_duplicate(story_id);
    }
}

void set_story_full_title(char *story_full_title)
{
    if (level_2_story_view.story_full_title)
    {
        free(level_2_story_view.story_full_title);
        level_2_story_view.story_full_title = NULL;
    }
    if (story_full_title)
    {
        level_2_story_view.story_full_title = string_duplicate(story_full_title);
    }
    main_story_text_window_ui_update();
}

void set_story_short_summary(char *story_short_summary)
{
    if (level_2_story_view.story_short_summary)
    {
        free(level_2_story_view.story_short_summary);
        level_2_story_view.story_short_summary = NULL;
    }
    if (story_short_summary)
    {
        level_2_story_view.story_short_summary = string_duplicate(story_short_summary);
    }
    main_story_text_window_ui_update();
}

void set_story_read(uint16_t index, bool story_read)
{
    if (index < level_2_story_view.story_titles_count)
    {
        level_2_story_view.stories_read[index] = story_read;
    }
    main_story_list_window_ui_update();
}

void push_story_read_statuses(char *stories_read_string)
{
    char *temp_array[get_story_list_data()->story_titles_count];
    string_split_to_array(temp_array, 0, get_story_list_data()->story_titles_count, stories_read_string, "||");
    uint16_t index = 0;
    while (index < get_story_list_data()->story_titles_count && temp_array[index] != NULL)
    {
        level_2_story_view.stories_read[index] = (strcmp(temp_array[index], "1") == 0);
        index++;
    }
    main_story_list_window_ui_update();
}

const StoryData *get_story_data(void)
{
    static StoryData data;
    data.story_id = level_2_story_view.story_id ? level_2_story_view.story_id : "";
    data.story_full_title = level_2_story_view.story_full_title ? level_2_story_view.story_full_title
                                                                : (char *)localization_get_string(STRING_LOADING);
    data.story_short_summary = level_2_story_view.story_short_summary ? level_2_story_view.story_short_summary
                                                                      : (char *)localization_get_string(STRING_LOADING);
    return &data;
}

void clear_story_data(void)
{
    if (level_2_story_view.story_id)
    {
        free(level_2_story_view.story_id);
        level_2_story_view.story_id = NULL;
    }
    if (level_2_story_view.story_full_title)
    {
        free(level_2_story_view.story_full_title);
        level_2_story_view.story_full_title = NULL;
    }
    if (level_2_story_view.story_short_summary)
    {
        free(level_2_story_view.story_short_summary);
        level_2_story_view.story_short_summary = NULL;
    }
    main_story_text_window_ui_update();
}

// ============================================================================
// Level 3: Available Details Implementation
// ============================================================================

void set_available_details(char *details_string)
{
    if (level_3_available_details.available_details_count > 0)
    {
        clear_available_details_data();
    }
    level_3_available_details.available_details_count =
        string_split_to_array(level_3_available_details.available_details,
                              level_3_available_details.available_details_count, MAX_ITEMS_LOW, details_string, "||");
    request_available_details_menu_update();
}

const AvailableDetailsData *get_available_details_data(void)
{
    static AvailableDetailsData data;
    if (level_3_available_details.available_details_count == 0)
    {
        static const char *loading_arr[1];
        loading_arr[0] = localization_get_string(STRING_LOADING);
        data.available_details = (char **)loading_arr;
        data.available_details_count = 1;
    }
    else
    {
        data.available_details = level_3_available_details.available_details;
        data.available_details_count = level_3_available_details.available_details_count;
    }
    return &data;
}

void clear_available_details_data(void)
{
    for (int i = 0; i < level_3_available_details.available_details_count; ++i)
    {
        if (level_3_available_details.available_details[i])
        {
            free(level_3_available_details.available_details[i]);
            level_3_available_details.available_details[i] = NULL;
        }
    }
    level_3_available_details.available_details_count = 0;

    // Also clear the detail_type since we're going back to the available details list
    if (level_4_selected_detail.detail_type)
    {
        free(level_4_selected_detail.detail_type);
        level_4_selected_detail.detail_type = NULL;
    }

    request_available_details_menu_update();
}

// ============================================================================
// Level 4: Selected Detail Implementation
// ============================================================================

void set_detail_type(char *type, void (*callback)(void))
{
    if (level_4_selected_detail.detail_type)
    {
        free(level_4_selected_detail.detail_type);
        level_4_selected_detail.detail_type = NULL;
    }
    if (type)
    {
        level_4_selected_detail.detail_type = string_duplicate(type);
    }
    if (callback)
    {
        callback();
    }
}

void set_detail_options(char *options_string, void (*callback)(void))
{
    DEBUG_LOG("KNCData", "set_detail_options called with: %s", options_string ? options_string : "NULL");
    if (level_4_selected_detail.detail_options_count > 0)
    {
        clear_detail_list_data();
    }
    level_4_selected_detail.detail_options_count =
        string_split_to_array(level_4_selected_detail.detail_options, level_4_selected_detail.detail_options_count,
                              MAX_ITEMS_LOW, options_string, "||");
    if (callback)
    {
        callback();
    }
}

const DetailListData *get_detail_list_data(void)
{
    static DetailListData data;
    data.detail_type = level_4_selected_detail.detail_type;
    DEBUG_LOG("KNCData", "get_detail_list_data: count=%d, type=%s", level_4_selected_detail.detail_options_count,
              level_4_selected_detail.detail_type ? level_4_selected_detail.detail_type : "NULL");
    if (level_4_selected_detail.detail_options_count == 0)
    {
        static const char *loading_arr[1];
        loading_arr[0] = localization_get_string(STRING_LOADING);
        data.detail_options = (char **)loading_arr;
        data.detail_options_count = 1;
    }
    else
    {
        data.detail_options = level_4_selected_detail.detail_options;
        data.detail_options_count = level_4_selected_detail.detail_options_count;
    }
    return &data;
}

void clear_detail_list_data(void)
{
    DEBUG_LOG("KNCData", "clear_detail_list_data called");
    // NOTE: We do NOT clear detail_type here because it needs to persist
    // when transitioning from detail list (tier 4 list) to detail text (tier 4 text).
    // The detail_type is cleared by clear_available_details_data() when going back to tier 3.

    for (int i = 0; i < level_4_selected_detail.detail_options_count; ++i)
    {
        if (level_4_selected_detail.detail_options[i])
        {
            free(level_4_selected_detail.detail_options[i]);
            level_4_selected_detail.detail_options[i] = NULL;
        }
    }
    level_4_selected_detail.detail_options_count = 0;
}

void set_detail_title(char *title, void (*callback)(void))
{
    if (level_4_selected_detail.detail_title)
    {
        free(level_4_selected_detail.detail_title);
        level_4_selected_detail.detail_title = NULL;
    }
    if (title)
    {
        level_4_selected_detail.detail_title = string_duplicate(title);
    }
    if (callback)
    {
        callback();
    }
}

void set_detail_text(char *text, void (*callback)(void))
{
    if (level_4_selected_detail.detail_text)
    {
        free(level_4_selected_detail.detail_text);
        level_4_selected_detail.detail_text = NULL;
    }
    if (text)
    {
        level_4_selected_detail.detail_text = string_duplicate(text);
    }
    if (callback)
    {
        callback();
    }
}

const DetailTextData *get_detail_text_data(void)
{
    static DetailTextData data;
    data.detail_title = level_4_selected_detail.detail_title ? level_4_selected_detail.detail_title
                                                             : (char *)localization_get_string(STRING_LOADING);
    data.detail_text = level_4_selected_detail.detail_text ? level_4_selected_detail.detail_text
                                                           : (char *)localization_get_string(STRING_LOADING);
    return &data;
}

void clear_detail_text_data(void)
{
    if (level_4_selected_detail.detail_title)
    {
        free(level_4_selected_detail.detail_title);
        level_4_selected_detail.detail_title = NULL;
    }
    if (level_4_selected_detail.detail_text)
    {
        free(level_4_selected_detail.detail_text);
        level_4_selected_detail.detail_text = NULL;
    }
}

void set_detail_has_sources(bool has_sources, void (*callback)(void))
{
    level_4_selected_detail.has_sources = has_sources;
    if (callback)
    {
        callback();
    }
}

void set_detail_sources(char *sources, void (*callback)(void))
{
    if (level_4_selected_detail.sources_count > 0)
    {
        clear_detail_sources_data();
    }
    level_4_selected_detail.sources_count = string_split_to_array(
        level_4_selected_detail.sources, level_4_selected_detail.sources_count, MAX_ITEMS_LOW, sources, "||");
    if (callback)
    {
        callback();
    }
}

const DetailSourcesData *get_detail_sources_data(void)
{
    static DetailSourcesData data;
    data.has_sources = level_4_selected_detail.has_sources;
    DEBUG_LOG("KNCData", "get_detail_sources_data: count=%d, has_sources=%d", level_4_selected_detail.sources_count,
              level_4_selected_detail.has_sources);
    ;
    if (level_4_selected_detail.sources_count == 0)
    {
        static const char *loading_arr[1];
        loading_arr[0] = localization_get_string(STRING_LOADING);
        data.sources = (char **)loading_arr;
        data.sources_count = 1;
    }
    else
    {
        data.sources = level_4_selected_detail.sources;
        data.sources_count = level_4_selected_detail.sources_count;
    }
    return &data;
}

void clear_detail_sources_data(void)
{
    DEBUG_LOG("KNCData", "clear_detail_sources_data called, count was: %d", level_4_selected_detail.sources_count);
    level_4_selected_detail.has_sources = false;
    for (int i = 0; i < level_4_selected_detail.sources_count; ++i)
    {
        if (level_4_selected_detail.sources[i])
        {
            free(level_4_selected_detail.sources[i]);
            level_4_selected_detail.sources[i] = NULL;
        }
    }
    level_4_selected_detail.sources_count = 0;
}

void push_qr_code_chunk(uint8_t *chunk, uint16_t chunk_length)
{
    if (chunk == NULL || chunk_length == 0)
        return;

    uint16_t offset = level_4_selected_detail.qr_code_chunks_count;
    DEBUG_LOG("KNCData", "push_qr_code_chunk: offset=%d, chunk_length=%d", offset, chunk_length);

    // Allocate buffer on first use with a fixed maximum capacity
    if (level_4_selected_detail.qr_code_chunks == NULL)
    {
        level_4_selected_detail.qr_code_chunks_capacity = QR_MAX_CAPACITY;
        level_4_selected_detail.qr_code_chunks = malloc(level_4_selected_detail.qr_code_chunks_capacity);
        if (!level_4_selected_detail.qr_code_chunks)
        {
            ERROR_LOG("KNCData", "Failed to allocate QR buffer");
            return;
        }
    }

    // Enforce maximum capacity
    if ((uint32_t)offset + (uint32_t)chunk_length > (uint32_t)level_4_selected_detail.qr_code_chunks_capacity ||
        (uint32_t)offset + (uint32_t)chunk_length > (uint32_t)QR_MAX_CAPACITY)
    {
        WARN_LOG("KNCData", "QR buffer overflow prevented: capacity=%d, required=%d",
                 level_4_selected_detail.qr_code_chunks_capacity, offset + chunk_length);
        return;
    }

    memcpy(level_4_selected_detail.qr_code_chunks + offset, chunk, chunk_length);
    level_4_selected_detail.qr_code_chunks_count += chunk_length;
    qr_view_refresh();
}

void set_qr_size(uint8_t size)
{
    level_4_selected_detail.qr_size = size;
    DEBUG_LOG("KNCData", "QR size set to: %d", size);
}

const QRCodeData *get_qr_code_data(void)
{
    static QRCodeData data;
    data.qr_code_chunks = level_4_selected_detail.qr_code_chunks ? level_4_selected_detail.qr_code_chunks : NULL;
    data.qr_code_chunks_count = level_4_selected_detail.qr_code_chunks_count;
    data.qr_size = level_4_selected_detail.qr_size;
    return &data;
}

bool qr_code_loaded(void)
{
    bool loaded = level_4_selected_detail.qr_code_chunks_count > 0;
    DEBUG_LOG("KNCData", "qr_code_loaded check: %d (count=%d)", loaded, level_4_selected_detail.qr_code_chunks_count);
    return loaded;
}

void clear_qr_code_data(void)
{
    DEBUG_LOG("KNCData", "clear_qr_code_data called, count was: %d", level_4_selected_detail.qr_code_chunks_count);
    if (level_4_selected_detail.qr_code_chunks)
    {
        free(level_4_selected_detail.qr_code_chunks);
        level_4_selected_detail.qr_code_chunks = NULL;
        level_4_selected_detail.qr_code_chunks_capacity = 0;
    }
    level_4_selected_detail.qr_code_chunks_count = 0;
    level_4_selected_detail.qr_size = 0;
    qr_view_refresh();
}

// ============================================================================
// Helper functions to clear specific navigation levels
// Use these when navigating back in the UI hierarchy
// ============================================================================

void clear_level_4_selected_detail(void)
{
    clear_detail_list_data();
    clear_detail_text_data();
    clear_detail_sources_data();
    clear_qr_code_data();
}

void clear_level_3_available_details(void)
{
    clear_available_details_data();
    clear_level_4_selected_detail(); // Also clear deeper levels
}

void clear_level_2_story_view(void)
{
    clear_story_list_data();
    clear_story_data();
    clear_level_3_available_details(); // Also clear deeper levels
}

void data_init(void)
{
}

void data_deinit(void)
{
    // Clean up Level 1: Categories
    clear_categories_data();

    // Clean up Level 2: Story View (includes story list + story text)
    clear_story_list_data();
    clear_story_data();

    // Clean up Level 3: Available Details
    clear_available_details_data();

    // Clean up Level 4: Selected Detail (includes detail list + detail text + sources)
    clear_detail_list_data();
    clear_detail_text_data();
    clear_detail_sources_data();
    clear_qr_code_data();
}