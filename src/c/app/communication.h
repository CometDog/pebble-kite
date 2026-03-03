#pragma once

#include <pebble.h>

/**
 * Send a request to get category names for a given page
 * @param page The page number to request
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_category_names(int8_t page);
/**
 * Send a request to get story titles for a given category and page
 * @param page The page number to request
 * @param category_name The name of the category to get stories for
 * @param short_data Whether to request short data only
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_story_titles(int8_t page, char *category_name, bool short_data);
/**
 * Send a request to get a short summary for a given story
 * @param category_name The name of the category the story belongs to
 * @param story_title The title of the story to get a summary for
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_short_summary(char *category_name, char *story_title);
/**
 * Send a request to get the reason for the current tension index
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_tension_index_reason(void);
/**
 * Send a request to get the QR code bitmap for a given article domain
 * @param article_domain The domain of the article to get the QR code for
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_qr_code_bitmap(char *article_domain);
/**
 * Send a request to get the next chunk of the QR code bitmap
 * @param chunk The chunk number to request
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_next_qr_code_bitmap(int8_t chunk);
/**
 * Send a request to get available details for a given story
 * @param category_name The name of the category the story belongs to
 * @param story_id The ID of the story to get available details for
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_story_available_details(char *category_name, char *story_id);
/**
 * Send a request to get detailed information for a given story
 * @param story_detail The detail identifier to request
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_story_detail(char *story_detail);
/**
 * Send a request to get the perspective text for a given POV
 * @param type The type of detail to get text for
 * @param key The key identifier of the detail to get text for
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_current_detail_text(char *type, char *key);
/**
 * Send a request to get story detail sources for a given text
 * @param text The text identifier to request sources for
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_get_story_detail_sources(char *type, char *key);

/**
 * Send a debug log message to the phone
 * @param level The log level (0=debug, 1=info, 2=warn, 3=error)
 * @param tag The tag/category for the log message
 * @param message The log message content
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_debug_log(int32_t level, const char *tag, const char *message);
/**
 * Send a debug notification to the phone
 * @param title The notification title
 * @param message The notification message
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_debug_notify(const char *title, const char *message);

/**
 * Send a request to mark a story as read
 * @param story_id The ID of the story to mark as read
 * @return true if the request was successfully queued/sent, false otherwise
 */
bool send_mark_story_read(const char *story_id);
/**
 * Initialize the communication module, setting up message routing and handlers
 */
void communication_init(void);
/**
 * Deinitialize the communication module, cleaning up resources
 */
void communication_deinit(void);
