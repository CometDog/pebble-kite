#pragma once

#include <pebble.h>

/**
 * Initialize the story articles UI
 */
void source_detail_list_window_ui_init(void);
/**
 * Deinitialize the story articles UI
 */
void source_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available articles menu, causing a redraw
 */
void source_detail_list_window_ui_update(void);