#pragma once

#include <pebble.h>

/**
 * Initialize and show the splash screen
 */
void splash_screen_init(void);

/**
 * Deinitialize and clean up the splash screen
 */
void splash_screen_deinit(void);

/**
 * Dismiss the splash screen and transition to categories
 * Call this when categories have finished loading
 */
void splash_screen_dismiss(void);

/**
 * Check if splash screen is currently showing
 * @return true if splash screen is visible
 */
bool splash_screen_is_showing(void);

/**
 * Update the splash screen progress bar.
 * @param current Current progress value
 * @param max Maximum progress value
 */
void splash_screen_set_loading_progress(uint16_t current, uint16_t max);
