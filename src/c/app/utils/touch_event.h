#pragma once
#include <pebble.h>

typedef void (*TouchEventCallback)(void *context);

/**
 * Subscribe to gestures.
 * No-op on platforms without a touchscreen or when touch is disabled.
 * @param on_swipe_up   Called when the user swipes up (finger moves upward)
 * @param on_swipe_down Called when the user swipes down (finger moves downward)
 * @param context       Passed through to each callback
 */
void touch_event_subscribe(TouchEventCallback on_swipe_up, TouchEventCallback on_swipe_down, void *context);

/**
 * Unsubscribe from gestures.
 */
void touch_event_unsubscribe(void);
