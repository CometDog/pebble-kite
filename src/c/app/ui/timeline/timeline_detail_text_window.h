#pragma once

#include <pebble.h>

/**
 * Initialize and show the timeline detail UI
 */
void timeline_detail_text_window_ui_init(void);
/**
 * Deinitialize the timeline detail UI
 */
void timeline_detail_text_window_ui_deinit(void);
/**
 * Update the timeline detail UI if visible
 */
void timeline_detail_text_window_ui_update(void);
