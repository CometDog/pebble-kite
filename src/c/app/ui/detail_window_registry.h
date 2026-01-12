#pragma once

#include "../detail_types.h"
#include <pebble.h>

/**
 * Initialize a detail list window for a specific detail type
 * @param detail_type The type of detail
 */
void detail_window_registry_init_list(DetailType detail_type);

/**
 * Initialize a detail text window for a specific detail type
 * @param detail_type The type of detail
 */
void detail_window_registry_init_text(DetailType detail_type);

/**
 * Deinitialize the current detail list window
 */
void detail_window_registry_deinit_list(void);

/**
 * Deinitialize the current detail text window
 */
void detail_window_registry_deinit_text(void);

/**
 * Update the current detail list window
 */
void detail_window_registry_update_list(void);

/**
 * Update the current detail text window
 */
void detail_window_registry_update_text(void);
