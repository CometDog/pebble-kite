#pragma once

#include "../data_manager.h"
#include "../detail_types.h"
#include <pebble.h>

/**
 * Callback type for item selection in detail list window
 * @param index: The index of the selected item
 */
typedef void (*DetailListSelectionCallback)(int index);

/**
 * Initialize the detail list window UI
 * @param window: pointer to the Window* to initialize
 * @param resource_requirements: array of DataResource enums required by this window
 * @param detail_type: The type of detail being displayed
 * @param on_select: Callback invoked when user selects an item (can be NULL)
 */
void detail_list_window_ui_init(Window **window, DataResource resource_requirements[], DetailType detail_type,
                                DetailListSelectionCallback on_select);

/**
 * Initialize a detail list window with default navigation behavior.
 * @param window: pointer to the Window* to initialize
 * @param detail_type: The type of detail being displayed
 */
void detail_list_window_ui_init_with_default_nav(Window **window, DetailType detail_type);

/**
 * Deinitialize the detail list window UI
 */
void detail_list_window_ui_deinit(Window **window);

/**
 * Request an update to the available detail list menu, causing a redraw
 */
void detail_list_window_ui_update(Window **window);

/**
 * Set the callback to check if an item is read
 * @param window The detail list window
 * @param callback The callback function
 */
void detail_list_window_set_is_read_callback(Window **window, bool (*callback)(int index));
