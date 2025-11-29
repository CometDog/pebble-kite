#pragma once

#include "../data_manager.h"
#include <pebble.h>

/**
 * Create a standard list menu window.
 *
 * @param window: pointer to a Window* that will receive the created window
 * @param title: window title
 * @param get_count: function returning number of items
 * @param get_items: function returning array of item strings
 * @param select_callback: function called with selected index
 *
 * Returns the created Window* or NULL on failure.
 */
Window *detail_list_create(Window **window, const char *title, uint16_t (*get_count)(void), char **(*get_items)(void),
                           void (*select_callback)(int));

/**
 * Convenience wrapper which also registers requested resources using data manager.
 * It registers the provided resources and returns the DataRegistrationHandle in handle.
 *
 * @param window: pointer to a Window* that will receive the created window
 * @param title: window title
 * @param get_count: function returning number of items
 * @param get_items: function returning array of item strings
 * @param select_callback: function called with selected index
 * @param resources: array of DataResource enums to register
 * @param resource_count: number of resources in the resources array
 * @param handle: pointer to DataRegistrationHandle that will receive the registration handle
 */
Window *detail_list_create_with_resources(Window **window, const char *title, uint16_t (*get_count)(void),
                                          char **(*get_items)(void), void (*select_callback)(int),
                                          DataResource *resources, size_t resource_count,
                                          DataRegistrationHandle *handle);
