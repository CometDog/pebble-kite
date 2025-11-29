#pragma once

#include "../data_manager.h"
#include <pebble.h>

/**
 * Create a standard detail text window.
 *
 * @param window: pointer to receive created Window*
 * @param get_title: function that returns the title string for the current selected item
 * @param get_text: function that returns the text body to display
 * @param detail_type: string identifying the detail type (e.g., "TalkingPoints", "Perspectives")
 * @param resources: array of DataResource enums required by this window
 * @param resource_count: number of resources
 * @param handle: optional out param to receive the DataRegistrationHandle
 *
 * Returns the created Window* or NULL on failure.
 */
Window *detail_text_create(Window **window, char *(*get_title)(void), char *(*get_text)(void), const char *detail_type,
                           DataResource *resources, size_t resource_count, DataRegistrationHandle *handle);

/**
 * Variant of detail_text_create that accepts an optional on_unload callback.
 * The on_unload callback will be invoked after the detail window's internal
 * unload cleanup runs.
 *
 * @param window: pointer to receive created Window*
 * @param get_title: function that returns the title string for the current selected item
 * @param get_text: function that returns the text body to display
 * @param detail_type: string identifying the detail type (e.g., "TalkingPoints", "Perspectives")
 * @param resources: array of DataResource enums required by this window
 * @param resource_count: number of resources
 * @param handle: optional out param to receive the DataRegistrationHandle
 * @param on_unload: optional callback invoked after internal unload cleanup
 */
Window *detail_text_create_with_unload(Window **window, char *(*get_title)(void), char *(*get_text)(void),
                                       const char *detail_type, DataResource *resources, size_t resource_count,
                                       DataRegistrationHandle *handle, void (*on_unload)(void));

/**
 * Destroy a detail text window created by detail_text_create.
 *
 * @param window: the detail text Window* to destroy
 */
void detail_text_destroy(Window *window);

/**
 * Request the detail text window to refresh its content. This should be
 * called when data has been updated and the window is visible.
 *
 * @param window: the detail text Window* to update
 */
void detail_text_update(Window *window);

/**
 * Set the callback to be invoked when the select button is pressed.
 * Only called when has_next_page is true.
 *
 * @param window: the detail text Window*
 * @param on_select: callback function to invoke on select button press
 */
void detail_text_set_select_handler(Window *window, void (*on_select)(void));
