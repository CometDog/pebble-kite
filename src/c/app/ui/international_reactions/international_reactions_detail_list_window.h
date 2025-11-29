#pragma once

#include <pebble.h>

/**
 * Initialize the international reactions list UI
 */
void international_reactions_detail_list_window_ui_init(void);
/**
 * Deinitialize the international reactions list UI
 */
void international_reactions_detail_list_window_ui_deinit(void);
/**
 * Request an update to the international reactions menu, causing a redraw
 */
void international_reactions_detail_list_window_ui_update(void);
