#pragma once

#include <pebble.h>

typedef void (*MessageHandler)(DictionaryIterator *iter);

/**
 * Initialize the message router
 */
void message_router_init(void);
/**
 * Clean up the message router
 */
void message_router_deinit(void);
/**
 * Register a message handler for a specific message type
 * @param type_name The message type to handle (e.g., "get_category_names")
 * @param handler Function to handle the message
 */
void message_router_register(const char *type_name, MessageHandler handler);
/**
 * Handle an incoming message by routing it to the appropriate handler
 * @param iter Dictionary iterator containing the message
 */
void message_router_handle_message(DictionaryIterator *iter);