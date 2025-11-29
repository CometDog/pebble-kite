#pragma once

#include <pebble.h>

/**
 * Initialize and show the industry impact detail text window UI
 */
void industry_impact_detail_text_window_ui_init(void);
/**
 * Deinitialize the industry impact detail text window UI
 */
void industry_impact_detail_text_window_ui_deinit(void);
/**
 * Update the industry impact detail text window UI if visible
 */
void industry_impact_detail_text_window_ui_update(void);