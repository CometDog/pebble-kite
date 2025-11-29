#pragma once

#include <pebble.h>

/**
 * Initialize the story technical details UI
 */
void technical_detail_detail_list_window_ui_init(void);
/**
 * Deinitialize the story technical details UI
 */
void technical_detail_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available technical details menu, causing a redraw
 */
void technical_detail_detail_list_window_ui_update(void);
