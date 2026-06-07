#include "touch_event.h"

#define TOUCH_SCROLL_Y_THRESHOLD 20 // y-pixels

#if defined(PBL_TOUCH)
static TouchEventCallback s_on_swipe_up = NULL;
static TouchEventCallback s_on_swipe_down = NULL;
static void *s_context = NULL;
static int16_t s_reference_y = 0;

typedef enum
{
    TouchDir_None = 0,
    TouchDir_Up = -1,
    TouchDir_Down = 1
} TouchDir;

static TouchDir s_last_dir = TouchDir_None;

static void touch_handler(const TouchEvent *event, void *context)
{
    switch (event->type)
    {
    case TouchEvent_Touchdown:
        s_reference_y = event->y;
        s_last_dir = TouchDir_None;
        break;
    case TouchEvent_PositionUpdate: {
        int16_t delta_y = event->y - s_reference_y;
        if (delta_y < -TOUCH_SCROLL_Y_THRESHOLD)
        {
            if (s_last_dir != TouchDir_Up && s_on_swipe_up)
            {
                s_on_swipe_up(s_context);
                s_last_dir = TouchDir_Up;
            }
            s_reference_y = event->y;
        }
        else if (delta_y > TOUCH_SCROLL_Y_THRESHOLD)
        {
            if (s_last_dir != TouchDir_Down && s_on_swipe_down)
            {
                s_on_swipe_down(s_context);
                s_last_dir = TouchDir_Down;
            }
            s_reference_y = event->y;
        }
        break;
    }
    case TouchEvent_Liftoff:
        s_last_dir = TouchDir_None;
        break;
    }
}
#endif

void touch_event_subscribe(TouchEventCallback on_swipe_up, TouchEventCallback on_swipe_down, void *context)
{
#if defined(PBL_TOUCH)
    s_on_swipe_up = on_swipe_up;
    s_on_swipe_down = on_swipe_down;
    s_context = context;
    s_reference_y = 0;
    // Doing this could cause some misbehavior if I ever handle cross-screen events that use tap
    // since the "next" screen will subscribe with the user's finger already down and TouchDir to
    // None which could be how I register "taps"
    // I should cross that bridge when I get there
    s_last_dir = TouchDir_None;
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
    s_context = NULL;
#endif
}
