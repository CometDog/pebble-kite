#pragma once
#include <pebble.h>

/**
 * Initialize the travel advisory list UI
 */
void travel_advisory_detail_list_window_ui_init(void);
/**
 * Deinitialize the travel advisory list UI
 */
void travel_advisory_detail_list_window_ui_deinit(void);
/**
 * Request a refresh of the travel advisory menu
 */
void travel_advisory_detail_list_window_ui_update(void);
