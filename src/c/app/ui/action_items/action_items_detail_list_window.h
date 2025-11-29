#pragma once

#include <pebble.h>

/**
 * Initialize the action items list UI
 */
void action_items_detail_list_window_ui_init(void);
/**
 * Deinitialize the action items list UI
 */
void action_items_detail_list_window_ui_deinit(void);
/**
 * Request an update to the action items menu, causing a redraw
 */
void action_items_detail_list_window_ui_update(void);
