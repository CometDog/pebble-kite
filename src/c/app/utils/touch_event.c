#include "touch_event.h"

#if defined(PBL_TOUCH)
#define TOUCH_SCROLL_X_THRESHOLD 20      // x-pixels
#define TOUCH_SCROLL_Y_THRESHOLD 20      // y-pixels
#define TOUCH_EVENT_DEBOUNCE_TIMEOUT 100 // ms

static TouchEventCallback s_on_swipe_up = NULL;
static TouchEventCallback s_on_swipe_down = NULL;
static TouchEventCallback s_on_swipe_left = NULL;
static TouchEventCallback s_on_swipe_right = NULL;
static void *s_context = NULL;
static int16_t s_reference_y = 0;
static int16_t s_reference_x = 0;
static bool s_touch_paused = false;

typedef enum
{
    TouchDir_None = 0,
    TouchDir_Up = -1,
    TouchDir_Down = 1,
    TouchDir_Left = -2,
    TouchDir_Right = 2
} TouchDir;

static TouchDir s_last_dir = TouchDir_None;

void touch_event_debounce_app_timer_callback(void *context)
{
    s_touch_paused = false;
}

void touch_event_debounce()
{
    s_touch_paused = true;
    app_timer_register(TOUCH_EVENT_DEBOUNCE_TIMEOUT, touch_event_debounce_app_timer_callback, NULL);
}

static void touch_handler(const TouchEvent *event, void *context)
{
    switch (event->type)
    {
    case TouchEvent_Touchdown:
        if (s_touch_paused)
        {
            break;
        }
        s_reference_x = event->x;
        s_reference_y = event->y;
        s_last_dir = TouchDir_None;
        break;
    case TouchEvent_PositionUpdate: {
        if (s_touch_paused)
        {
            break;
        }
        if (s_last_dir == TouchDir_None)
        {
            int16_t delta_y = event->y - s_reference_y;
            int16_t delta_x = event->x - s_reference_x;
            if (s_on_swipe_up && delta_y < -TOUCH_SCROLL_Y_THRESHOLD)
            {
                s_last_dir = TouchDir_Up;
                s_reference_y = event->y;
            }
            else if (s_on_swipe_down && delta_y > TOUCH_SCROLL_Y_THRESHOLD)
            {
                s_last_dir = TouchDir_Down;
                s_reference_y = event->y;
            }
            else if (s_on_swipe_left && delta_x < -TOUCH_SCROLL_X_THRESHOLD)
            {
                s_last_dir = TouchDir_Left;
                s_reference_x = event->x;
            }
            else if (s_on_swipe_right && delta_x > TOUCH_SCROLL_X_THRESHOLD)
            {
                s_last_dir = TouchDir_Right;
                s_reference_x = event->x;
            }
        }
        break;
    }
    case TouchEvent_Liftoff:
        touch_event_debounce();
        switch (s_last_dir)
        {
        case TouchDir_Up:
            if (s_on_swipe_up != NULL)
            {
                s_on_swipe_up(s_context);
            }
            break;
        case TouchDir_Down:
            if (s_on_swipe_down != NULL)
            {
                s_on_swipe_down(s_context);
            }
            break;
        case TouchDir_Left:
            if (s_on_swipe_left != NULL)
            {
                s_on_swipe_left(s_context);
            }
            break;
        case TouchDir_Right:
            if (s_on_swipe_right != NULL)
            {
                s_on_swipe_right(s_context);
            }
            break;
        default:
            break;
        }
        s_last_dir = TouchDir_None;
        break;
    }
}
#endif

void touch_event_subscribe(TouchEventCallback on_swipe_up, TouchEventCallback on_swipe_down,
                           TouchEventCallback on_swipe_left, TouchEventCallback on_swipe_right, void *context)
{
#if defined(PBL_TOUCH)
    s_on_swipe_up = on_swipe_up;
    s_on_swipe_down = on_swipe_down;
    s_on_swipe_left = on_swipe_left;
    s_on_swipe_right = on_swipe_right;
    s_context = context;
    s_reference_y = 0;
    s_reference_x = 0;
    s_last_dir = TouchDir_None;

    // Always start with touch paused and unpause it on resume
    touch_event_debounce();

    if (touch_service_is_enabled())
    {
        touch_service_subscribe(touch_handler, NULL);
    }
#endif
}

void touch_event_unsubscribe(void)
{
#if defined(PBL_TOUCH)
    touch_service_unsubscribe();
    s_on_swipe_up = NULL;
    s_on_swipe_down = NULL;
    s_on_swipe_left = NULL;
    s_on_swipe_right = NULL;
    s_touch_paused = false;
    s_context = NULL;
    s_reference_x = 0;
    s_reference_y = 0;
#endif
}