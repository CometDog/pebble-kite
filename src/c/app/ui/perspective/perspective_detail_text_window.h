#pragma once

#include <pebble.h>

/**
 * Initialize and show the perspective detail UI
 */
void perspective_detail_text_window_ui_init(void);
/**
 * Deinitialize the perspective detail UI
 */
void perspective_detail_text_window_ui_deinit(void);
/**
 * Update the perspective detail UI if visible
 */
void perspective_detail_text_window_ui_update(void);
