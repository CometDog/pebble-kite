#pragma once

#include <pebble.h>

/**
 * Initialize the story industry impact UI
 */
void industry_impact_detail_list_window_ui_init(void);
/**
 * Deinitialize the story industry impact UI
 */
void industry_impact_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available industry impact menu, causing a redraw
 */
void industry_impact_detail_list_window_ui_update(void);
