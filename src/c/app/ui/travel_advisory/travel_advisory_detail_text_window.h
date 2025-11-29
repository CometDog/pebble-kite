#pragma once

#include <pebble.h>

/**
 * Initialize and show the travel advisory detail UI
 */
void travel_advisory_detail_text_window_ui_init(void);
/**
 * Deinitialize the travel advisory detail UI
 */
void travel_advisory_detail_text_window_ui_deinit(void);
/**
 * Update the travel advisory detail UI if visible
 */
void travel_advisory_detail_text_window_ui_update(void);
