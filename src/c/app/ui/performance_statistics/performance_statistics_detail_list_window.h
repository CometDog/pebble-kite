#pragma once

#include <pebble.h>

/**
 * Initialize the performance statistics list UI
 */
void performance_statistics_detail_list_window_ui_init(void);
/**
 * Deinitialize the performance statistics list UI
 */
void performance_statistics_detail_list_window_ui_deinit(void);
/**
 * Request an update to the performance statistics menu, causing a redraw
 */
void performance_statistics_detail_list_window_ui_update(void);
