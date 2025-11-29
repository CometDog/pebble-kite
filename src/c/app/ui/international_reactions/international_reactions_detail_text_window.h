#pragma once

#include <pebble.h>

/**
 * Initialize and show the international reactions detail UI
 */
void international_reactions_detail_text_window_ui_init(void);
/**
 * Deinitialize the international reactions detail UI
 */
void international_reactions_detail_text_window_ui_deinit(void);
/**
 * Update the international reactions detail UI if visible
 */
void international_reactions_detail_text_window_ui_update(void);
