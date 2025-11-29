#pragma once

#include <pebble.h>

/**
 * Initialize the categories UI
 */
void categories_ui_init(void);
/**
 * Deinitialize the categories UI
 */
void categories_ui_deinit(void);
/**
 * Request an update to the categories menu, causing a redraw
 */
void request_categories_menu_update(void);