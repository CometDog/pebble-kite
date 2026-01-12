#pragma once

#include <pebble.h>

// Public data structures for grouped access
typedef struct
{
    char **categories;
    uint16_t categories_count;
} CategoriesData;

typedef struct
{
    char *selected_category;
    int selected_category_index;
    char **story_titles;
    bool *stories_read;
    uint16_t story_titles_count;
} StoryListData;

typedef struct
{
    char *story_id;
    char *story_full_title;
    char *story_short_summary;
} StoryData;

typedef struct
{
    char **available_details;
    uint16_t available_details_count;
} AvailableDetailsData;

typedef struct
{
    char *detail_type;
    char **detail_options;
    uint16_t detail_options_count;
} DetailListData;

typedef struct
{
    char *detail_title;
    char *detail_text;
} DetailTextData;

typedef struct
{
    bool has_sources;
    char **sources;
    uint16_t sources_count;
} DetailSourcesData;

typedef struct
{
    uint8_t *qr_code_chunks;
    uint16_t qr_code_chunks_count;
    uint8_t qr_size;
} QRCodeData;

/**
 * Initialize the data module, setting up internal structures
 */
void data_init(void);
/**
 * Deinitialize the data module, cleaning up resources
 */
void data_deinit(void);

/**
 * Helper functions to clear specific navigation levels
 * Use these when navigating back in the UI hierarchy to clear data
 * at the appropriate level and all deeper levels
 */

/**
 * Clear Level 4: Selected detail (type, options, title, text, sources, QR code)
 */
void clear_level_4_selected_detail(void);
/**
 * Clear Level 3: Available details list and all deeper levels (4)
 */
void clear_level_3_available_details(void);
/**
 * Clear Level 2: Story view (list, selected story data) and all deeper levels (3, 4)
 */
void clear_level_2_story_view(void);
// ============================================================================
// Level 1: Categories API
// ============================================================================

/**
 * Set the count of additional feeds included in the categories
 * @param count The number of additional feeds
 */
void set_additional_feeds_count(uint8_t count);

/**
 * Get the count of additional feeds included in the categories
 * @return The number of additional feeds
 */
uint8_t get_additional_feeds_count(void);

/**
 * Push new categories data from a received string
 * @param categories_string Pipe-separated categories string
 */
void push_local_categories(char *categories_string);

/**
 * Get categories data (read-only)
 * @return Pointer to categories data structure
 */
const CategoriesData *get_categories_data(void);

/**
 * Check if categories have been loaded
 * @return true if categories are loaded, false otherwise
 */
bool categories_loaded(void);

/**
 * Clear all categories data
 */
void clear_categories_data(void);

// ============================================================================
// Level 2: Story List + Story Data API
// ============================================================================

/**
 * Set the selected category name
 * @param category_name The name of the selected category
 */
void set_selected_category(char *category_name);

/**
 * Set the selected category index
 * @param index The index of the selected category
 */
void set_selected_category_index(int index);

/**
 * Push new story titles data from a received string
 * @param story_titles_string Pipe-separated story titles string
 */
void push_story_titles(char *story_titles_string);

/**
 * Get story list data (read-only)
 * @return Pointer to story list data structure
 */
const StoryListData *get_story_list_data(void);

/**
 * Clear story list data
 */
void clear_story_list_data(void);

/**
 * Set the current story's ID
 * @param story_id The story ID string
 */
void set_story_id(char *story_id);

/**
 * Set the current story's full title
 * @param story_full_title The full title string
 */
void set_story_full_title(char *story_full_title);

/**
 * Set the current story's short summary
 * @param story_short_summary The short summary string
 */
void set_story_short_summary(char *story_short_summary);

/**
 * Set the current story's read status
 * @param index Index of the story in the list
 * @param story_read Boolean indicating if the story has been read
 */
void set_story_read(uint16_t index, bool story_read);

/**
 * Push story read status in batches
 * @param stories_read_string String of double-pipe-separated "0" (unread) and "1" (read) values
 */
void push_story_read_statuses(char *stories_read_string);

/**
 * Get story data (read-only)
 * @return Pointer to story data structure
 */
const StoryData *get_story_data(void);

/**
 * Clear story data
 */
void clear_story_data(void);

// ============================================================================
// Level 3: Available Details API
// ============================================================================

/**
 * Set available details for the current story from a received string
 * @param details_string Pipe-separated details string
 */
void set_available_details(char *details_string);

/**
 * Get available details data (read-only)
 * @return Pointer to available details data structure
 */
const AvailableDetailsData *get_available_details_data(void);

/**
 * Clear available details data
 */
void clear_available_details_data(void);

// ============================================================================
// Level 4: Selected Detail API
// ============================================================================

/**
 * Set the current detail type
 * @param type The detail type string
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_type(char *type, void (*callback)(void));

/**
 * Set options for the current detail from a received string
 * @param options_string Pipe-separated detail options string
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_options(char *options_string, void (*callback)(void));

/**
 * Get detail list data (read-only)
 * @return Pointer to detail list data structure
 */
const DetailListData *get_detail_list_data(void);

/**
 * Clear detail list data
 */
void clear_detail_list_data(void);

/**
 * Set the current detail title
 * @param title The detail title string
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_title(char *title, void (*callback)(void));

/**
 * Set the current detail text
 * @param text The detail text string
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_text(char *text, void (*callback)(void));

/**
 * Get detail text data (read-only)
 * @return Pointer to detail text data structure
 */
const DetailTextData *get_detail_text_data(void);

/**
 * Clear detail text data
 */
void clear_detail_text_data(void);

/**
 * Set the current detail's source availability
 * @param has_sources Boolean indicating if the current detail has sources
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_has_sources(bool has_sources, void (*callback)(void));

/**
 * Set the current detail sources from a received string
 * @param sources Pipe-separated sources string
 * @param callback Optional callback function to invoke after setting
 */
void set_detail_sources(char *sources, void (*callback)(void));

/**
 * Get detail sources data (read-only)
 * @return Pointer to detail sources data structure
 */
const DetailSourcesData *get_detail_sources_data(void);

/**
 * Clear detail sources data
 */
void clear_detail_sources_data(void);

/**
 * Push a chunk of QR code bitmap data
 * @param chunk Array of bytes representing the bitmap chunk
 * @param chunk_length Length of the chunk in bytes
 */
void push_qr_code_chunk(uint8_t *chunk, uint16_t chunk_length);

/**
 * Set the QR code size (width/height in modules)
 * @param size The QR code dimension (e.g., 21, 25, 29, etc.)
 */
void set_qr_size(uint8_t size);

/**
 * Get QR code data (read-only)
 * @return Pointer to QR code data structure
 */
const QRCodeData *get_qr_code_data(void);

/**
 * Check if a QR code has been loaded
 * @return true if a QR code is loaded, false otherwise
 */
bool qr_code_loaded(void);

/**
 * Clear QR code data
 */
void clear_qr_code_data(void);