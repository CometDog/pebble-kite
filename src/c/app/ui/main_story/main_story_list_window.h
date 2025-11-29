#pragma once

#include <pebble.h>

/**
 * Initialize the stories UI for a given category
 */
void main_story_list_window_ui_init();
/**
 * Deinitialize the stories UI and clean up resources
 */
void main_story_list_window_ui_deinit(void);
/**
 * Request an update to the stories menu, causing a redraw
 */
void main_story_list_window_ui_update(void);
