#pragma once

#include <pebble.h>

/**
 * Initialize the story perspectives UI
 */
void perspective_detail_list_window_ui_init(void);
/**
 * Deinitialize the story perspectives UI
 */
void perspective_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available perspectives menu, causing a redraw
 */
void perspective_detail_list_window_ui_update(void);
