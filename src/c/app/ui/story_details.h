#pragma once

#include <pebble.h>

/**
 * Initialize the story details UI
 */
void story_details_ui_init(void);
/**
 * Deinitialize the story details UI
 */
void story_details_ui_deinit(void);
/**
 * Request an update to the available details menu, causing a redraw
 */
void request_available_details_menu_update(void);