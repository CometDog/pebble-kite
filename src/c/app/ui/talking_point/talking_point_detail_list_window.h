#pragma once

#include <pebble.h>

/**
 * Initialize the story talking points UI
 */
void talking_point_detail_list_window_ui_init(void);
/**
 * Deinitialize the story talking points UI
 */
void talking_point_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available talking points menu, causing a redraw
 */
void talking_point_detail_list_window_ui_update(void);
