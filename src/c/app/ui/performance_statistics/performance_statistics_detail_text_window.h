#pragma once

#include <pebble.h>

/**
 * Initialize and show the performance statistics detail UI
 */
void performance_statistics_detail_text_window_ui_init(void);
/**
 * Deinitialize the performance statistics detail UI
 */
void performance_statistics_detail_text_window_ui_deinit(void);
/**
 * Update the performance statistics detail UI if visible
 */
void performance_statistics_detail_text_window_ui_update(void);
