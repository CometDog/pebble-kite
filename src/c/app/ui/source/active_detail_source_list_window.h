#pragma once

#include <pebble.h>

/**
 * Initialize the detail articles UI
 */
void active_detail_source_list_window_ui_init(void);
/**
 * Deinitialize the detail articles UI
 */
void active_detail_source_list_window_ui_deinit(void);
/**
 * Request an update to the available articles menu, causing a redraw
 */
void active_detail_source_list_window_ui_update(void);