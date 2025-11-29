#pragma once

#include <pebble.h>

/**
 * Initialize the story timeline UI
 */
void timeline_detail_list_window_ui_init(void);
/**
 * Deinitialize the story timeline UI
 */
void timeline_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available timeline menu, causing a redraw
 */
void timeline_detail_list_window_ui_update(void);
