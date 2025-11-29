#pragma once

#include "../data_manager.h"
#include "../detail_types.h"
#include <pebble.h>

/**
 * Callback type for select button press in detail text window
 */
typedef void (*DetailTextSelectCallback)(void);

/**
 * Initialize and show the detail text window UI
 * @param window: pointer to the Window* to initialize
 * @param get_title: function that returns the title string
 * @param get_text: function that returns the text body
 * @param detail_type: string identifying the detail type (e.g., "TalkingPoints", "Perspectives")
 * @param resource_requirements: array of DataResource enums required by this window
 * @param on_select: Callback invoked when user presses select button (can be NULL)
 */
void detail_text_window_ui_init(Window **window, char *(*get_title)(void), char *(*get_text)(void),
                                const char *detail_type, DataResource resource_requirements[],
                                DetailTextSelectCallback on_select);

/**
 * Initialize a detail text window with standard data accessors and sources navigation.
 * @param window: pointer to the Window* to initialize
 * @param detail_type: The DetailType enum value for this window
 * @param set_title_on_init: If true, calls set_detail_title with the API name on init
 *                           (used for text-only types like Background, DidYouKnow)
 */
void detail_text_window_ui_init_with_default_handlers(Window **window, DetailType detail_type, bool set_title_on_init);

/**
 * Update a detail text window with standard sources-availability check.
 * Automatically sets up sources navigation if sources are available.
 * @param window: pointer to the Window* to update
 */
void detail_text_window_ui_update_with_sources_check(Window **window);

/**
 * Deinitialize the detail text window UI
 * @param window: pointer to the Window* to deinitialize
 */
void detail_text_window_ui_deinit(Window **window);

/**
 * Update the detail text window UI if visible
 * @param window: pointer to the Window* to update
 */
void detail_text_window_ui_update(Window **window);

/**
 * Set the callback handler for select button press
 * @param window: pointer to the Window*
 * @param on_select: callback function to invoke when select is pressed
 */
void detail_text_window_ui_set_select_handler(Window **window, void (*on_select)(void));