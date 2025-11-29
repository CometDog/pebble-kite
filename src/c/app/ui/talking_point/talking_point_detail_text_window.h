#pragma once

#include <pebble.h>

/**
 * Initialize and show the talking point detail UI
 */
void talking_point_detail_text_window_ui_init(void);
/**
 * Deinitialize the talking point detail UI
 */
void talking_point_detail_text_window_ui_deinit(void);
/**
 * Update the talking point detail UI if visible
 */
void talking_point_detail_text_window_ui_update(void);
