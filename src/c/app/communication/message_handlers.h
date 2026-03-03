#pragma once

#include <pebble.h>

/**
 * Handler for "provide_session_id" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_provide_session_id_message(DictionaryIterator *iter);
/**
 * Handler for "update_categories" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_update_categories_message(DictionaryIterator *iter);
/**
 * Handler for "get_category_names" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_category_names_message(DictionaryIterator *iter);
/**
 * Handler for "get_story_titles" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_story_titles_message(DictionaryIterator *iter);
/**
 * Handler for "get_short_summary" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_short_summary_message(DictionaryIterator *iter);
/**
 * Handler for "get_qr_code_bitmap" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_qr_code_bitmap_message(DictionaryIterator *iter);
/**
 * Handler for "get_story_available_details" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_story_available_details_message(DictionaryIterator *iter);
/**
 * Handler for "get_story_detail" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_story_detail_message(DictionaryIterator *iter);
/**
 * Handler for "get_story_detail_sources" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_get_story_detail_sources_message(DictionaryIterator *iter);
/**
 * Handler for "set_debug_mode" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_set_debug_mode_message(DictionaryIterator *iter);
/**
 * Handler for "send_interface_strings" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_send_interface_strings_message(DictionaryIterator *iter);

/**
 * Handler for "restart_app" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_restart_app_message(DictionaryIterator *iter);

/**
 * Handler for "set_text_size" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_set_text_size_message(DictionaryIterator *iter);
/**
 * Handler for "show_tension_index" message from phone
 * @param iter Dictionary iterator containing the message
 */
void handle_show_tension_index_message(DictionaryIterator *iter);