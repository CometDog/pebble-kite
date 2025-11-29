#pragma once

#include <pebble.h>

/**
 * Initialize and show the action items detail UI
 */
void action_items_detail_text_window_ui_init(void);
/**
 * Deinitialize the action items detail UI
 */
void action_items_detail_text_window_ui_deinit(void);
/**
 * Update the action items detail UI if visible
 */
void action_items_detail_text_window_ui_update(void);
