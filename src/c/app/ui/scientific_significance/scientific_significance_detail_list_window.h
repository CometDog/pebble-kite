#pragma once

#include <pebble.h>

/**
 * Initialize the scientific significance list UI
 */
void scientific_significance_detail_list_window_ui_init(void);
/**
 * Deinitialize the scientific significance list UI
 */
void scientific_significance_detail_list_window_ui_deinit(void);
/**
 * Request an update to the scientific significance menu, causing a redraw
 */
void scientific_significance_detail_list_window_ui_update(void);
