#include "message_router.h"
#include "../utils/debug_logger.h"

#define MAX_HANDLERS 20

typedef struct
{
    const char *type;
    MessageHandler handler;
} HandlerEntry;

static HandlerEntry s_handlers[MAX_HANDLERS];
static uint8_t s_handler_count = 0;

void message_router_init(void)
{
    s_handler_count = 0;
    memset(s_handlers, 0, sizeof(s_handlers));
}

void message_router_deinit(void)
{
    s_handler_count = 0;
    memset(s_handlers, 0, sizeof(s_handlers));
}

void message_router_register(const char *type_name, MessageHandler handler)
{
    if (s_handler_count >= MAX_HANDLERS)
    {
        ERROR_LOG("KNCMessageRouting", "Cannot register handler: maximum handlers reached");
        return;
    }

    s_handlers[s_handler_count] = (HandlerEntry){.type = type_name, .handler = handler};
    s_handler_count++;
}

void message_router_handle_message(DictionaryIterator *iter)
{
    if (!iter)
        return;

    Tuple *type_tuple = dict_find(iter, MESSAGE_KEY_type);
    if (!type_tuple)
    {
        ERROR_LOG("KNCMessageRouting", "Message received without type");
        return;
    }

    for (uint8_t i = 0; i < s_handler_count; i++)
    {
        if (strcmp(type_tuple->value->cstring, s_handlers[i].type) == 0)
        {
            s_handlers[i].handler(iter);
            return;
        }
    }

    WARN_LOG("KNCMessageRouting", "No handler found for message type: %s", type_tuple->value->cstring);
}