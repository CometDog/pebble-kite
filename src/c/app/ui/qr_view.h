#pragma once
#include <pebble.h>

/**
 * Initialize the QR code view UI
 */
void qr_view_init(void);
/**
 * Deinitialize the QR code view UI
 */
void qr_view_deinit(void);
/**
 * Refresh the QR code view, causing a redraw
 */
void qr_view_refresh(void);
