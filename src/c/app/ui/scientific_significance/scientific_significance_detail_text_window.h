#pragma once

#include <pebble.h>

/**
 * Initialize and show the scientific significance detail UI
 */
void scientific_significance_detail_text_window_ui_init(void);
/**
 * Deinitialize the scientific significance detail UI
 */
void scientific_significance_detail_text_window_ui_deinit(void);
/**
 * Update the scientific significance detail UI if visible
 */
void scientific_significance_detail_text_window_ui_update(void);
