#include "communication.h"
#include "communication/message_handlers.h"
#include "communication/message_router.h"
#include "utils/debug_logger.h"
#include <stdlib.h>
#include <string.h>

// APP_MESSAGE_OUTBOX_SIZE_MINIMUM is erroneously named; it is the maximum allowed size.
#define APP_MESSAGE_OUTBOX_SIZE_MAXIMUM APP_MESSAGE_OUTBOX_SIZE_MINIMUM
#define APP_MESSAGE_OUTBOX_SIZE 512
#define APP_MESSAGE_INBOX_SIZE 4096
#define COMMUNICATION_QUEUE_MAX 16

static void inbox_received_callback(DictionaryIterator *iter, void *context)
{
    message_router_handle_message(iter);
}

typedef enum
{
    VALUE_TYPE_STRING,
    VALUE_TYPE_INT
} ValueType;

typedef struct
{
    uint32_t key;
    ValueType type;
    union {
        const char *str;
        int32_t num;
    } value;
} ExtraRequestKeyValuePair;

typedef struct
{
    char *request;
    ExtraRequestKeyValuePair *pairs;
    size_t pair_count;
} QueuedMessage;

static QueuedMessage *g_queue[COMMUNICATION_QUEUE_MAX];
static int g_queue_head = 0;
static int g_queue_tail = 0;
static int g_queue_count = 0;

// Stand-in for missing strdup in Pebble SDK
static char *communication_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (!d)
        return NULL;
    memcpy(d, s, len);
    return d;
}

static void communication_free_queued(QueuedMessage *qm)
{
    if (!qm)
        return;
    if (qm->request)
        free(qm->request);
    if (qm->pairs)
    {
        for (size_t i = 0; i < qm->pair_count; i++)
        {
            if (qm->pairs[i].type == VALUE_TYPE_STRING && qm->pairs[i].value.str)
            {
                free((void *)qm->pairs[i].value.str);
            }
        }
        free(qm->pairs);
    }
    free(qm);
}

bool communication_enqueue_message(const char *request, const ExtraRequestKeyValuePair *extra_pairs, size_t pair_count)
{
    if (g_queue_count >= COMMUNICATION_QUEUE_MAX)
    {
        ERROR_LOG("KNCCommunication", "Queue full, cannot enqueue message");
        return false;
    }

    QueuedMessage *qm = malloc(sizeof(QueuedMessage));
    if (!qm)
        return false;
    qm->request = communication_strdup(request);
    qm->pair_count = pair_count;
    qm->pairs = NULL;

    if (pair_count > 0 && extra_pairs != NULL)
    {
        qm->pairs = malloc(sizeof(ExtraRequestKeyValuePair) * pair_count);
        if (!qm->pairs)
        {
            communication_free_queued(qm);
            return false;
        }
        for (size_t i = 0; i < pair_count; i++)
        {
            qm->pairs[i].key = extra_pairs[i].key;
            qm->pairs[i].type = extra_pairs[i].type;
            if (extra_pairs[i].type == VALUE_TYPE_STRING)
            {
                qm->pairs[i].value.str = communication_strdup(extra_pairs[i].value.str);
            }
            else
            {
                qm->pairs[i].value.num = extra_pairs[i].value.num;
            }
        }
    }

    g_queue[g_queue_tail] = qm;
    g_queue_tail = (g_queue_tail + 1) % COMMUNICATION_QUEUE_MAX;
    g_queue_count++;
    return true;
}

static bool communication_try_send_next(void)
{
    if (g_queue_count == 0)
        return false;

    QueuedMessage *qm = g_queue[g_queue_head];

    DictionaryIterator *iter;
    AppMessageResult begin_res = app_message_outbox_begin(&iter);
    if (begin_res == APP_MSG_BUSY)
    {
        return false;
    }
    if (begin_res != APP_MSG_OK)
    {
        ERROR_LOG("KNCCommunication", "Outbox begin failed when flushing queue: %d", begin_res);
        return false;
    }

    if (dict_write_cstring(iter, MESSAGE_KEY_type, qm->request) != DICT_OK)
    {
        ERROR_LOG("KNCCommunication", "Queue write request type failed");
        communication_free_queued(qm); // Drop the message to avoid infinite loop
        g_queue[g_queue_head] = NULL;
        g_queue_head = (g_queue_head + 1) % COMMUNICATION_QUEUE_MAX;
        g_queue_count--;
        return false;
    }

    if (qm->pairs != NULL)
    {
        for (size_t i = 0; i < qm->pair_count; i++)
        {
            ExtraRequestKeyValuePair *pair = &qm->pairs[i];
            if (pair->type == VALUE_TYPE_STRING)
            {
                if (dict_write_cstring(iter, pair->key, pair->value.str) != DICT_OK)
                {
                    ERROR_LOG("KNCCommunication", "Queue write string failed");
                    communication_free_queued(qm);
                    g_queue[g_queue_head] = NULL;
                    g_queue_head = (g_queue_head + 1) % COMMUNICATION_QUEUE_MAX;
                    g_queue_count--;
                    return false;
                }
            }
            else
            {
                if (dict_write_int(iter, pair->key, &pair->value.num, sizeof(int32_t), true) != DICT_OK)
                {
                    ERROR_LOG("KNCCommunication", "Queue write int failed");
                    communication_free_queued(qm);
                    g_queue[g_queue_head] = NULL;
                    g_queue_head = (g_queue_head + 1) % COMMUNICATION_QUEUE_MAX;
                    g_queue_count--;
                    return false;
                }
            }
        }
    }

    AppMessageResult send_res = app_message_outbox_send();

    if (send_res != APP_MSG_OK)
    {
        ERROR_LOG("KNCCommunication", "Outbox send failed when flushing queue: %d", send_res);
    }

    // Do not drop the message if busy; drop otherwise
    if (send_res == APP_MSG_BUSY)
    {
        return false;
    }

    // Sent; remove from queue
    communication_free_queued(qm);
    g_queue[g_queue_head] = NULL;
    g_queue_head = (g_queue_head + 1) % COMMUNICATION_QUEUE_MAX;
    g_queue_count--;
    return send_res == APP_MSG_OK;
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context)
{
    communication_try_send_next();
}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context)
{
    communication_try_send_next();
}

static bool send_request_with_extra(char *request, const ExtraRequestKeyValuePair *extra_pairs, size_t pair_count)
{
    AppMessageResult appMessageResult;
    DictionaryIterator *iter;

    appMessageResult = app_message_outbox_begin(&iter);
    if (appMessageResult == APP_MSG_BUSY)
    {
        bool enq_ok = false;
        extern bool communication_enqueue_message(const char *request, const ExtraRequestKeyValuePair *extra_pairs,
                                                  size_t pair_count);
        enq_ok = communication_enqueue_message(request, extra_pairs, pair_count);
        if (!enq_ok)
        {
            ERROR_LOG("KNCCommunication", "Queue enqueue failed when busy");
            return false;
        }
        INFO_LOG("KNCCommunication", "Outbox busy - queued message: %s", request);
        return true;
    }

    if (appMessageResult != APP_MSG_OK)
    {
        ERROR_LOG("KNCCommunication", "Outbox begin failed: %d", appMessageResult);
        return false;
    }

    if (dict_write_cstring(iter, MESSAGE_KEY_type, request) != DICT_OK)
    {
        ERROR_LOG("KNCCommunication", "Request type write failed");
        return false;
    }

    if (extra_pairs != NULL)
    {
        for (size_t i = 0; i < pair_count; i++)
        {
            const ExtraRequestKeyValuePair *pair = &extra_pairs[i];

            switch (pair->type)
            {
            case VALUE_TYPE_STRING:
                if (dict_write_cstring(iter, pair->key, pair->value.str) != DICT_OK)
                {
                    ERROR_LOG("KNCCommunication", "String write failed (key: %lu)", (unsigned long)pair->key);
                    return false;
                }
                break;

            case VALUE_TYPE_INT:
                if (dict_write_int(iter, pair->key, &pair->value.num, sizeof(int32_t), true) != DICT_OK)
                {
                    ERROR_LOG("KNCCommunication", "Int write failed (key: %lu, value: %ld)", (unsigned long)pair->key,
                              (long)pair->value.num);
                    ;
                    return false;
                }
                break;

            default:
                ERROR_LOG("KNCCommunication", "Unsupported value type (key: %lu)", (unsigned long)pair->key);
                return false;
            }
        }
    }

    if (app_message_outbox_send() != APP_MSG_OK)
    {
        ERROR_LOG("KNCCommunication", "Outbox send failed");
        return false;
    }

    return true;
}

static bool send_request(char *request)
{
    return send_request_with_extra(request, NULL, 0);
}

bool send_get_category_names(int8_t page)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_page, .type = VALUE_TYPE_INT, .value.num = page},
    };

    return send_request_with_extra("get_category_names", extras, 1);
}

bool send_get_story_titles(int8_t page, char *category_name, bool short_data)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_page, .type = VALUE_TYPE_INT, .value.num = page},
        {.key = MESSAGE_KEY_categoryName, .type = VALUE_TYPE_STRING, .value.str = category_name},
        {.key = MESSAGE_KEY_shortData, .type = VALUE_TYPE_INT, .value.num = short_data == true ? 1 : 0}};

    return send_request_with_extra("get_story_titles", extras, 3);
}

bool send_get_short_summary(char *category_name, char *story_title)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_categoryName, .type = VALUE_TYPE_STRING, .value.str = category_name},
        {.key = MESSAGE_KEY_storyTitle, .type = VALUE_TYPE_STRING, .value.str = story_title}};

    return send_request_with_extra("get_short_summary", extras, 2);
}

bool send_get_qr_code_bitmap(char *article_domain)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_articleDomain, .type = VALUE_TYPE_STRING, .value.str = article_domain}};

    return send_request_with_extra("get_qr_code_bitmap", extras, 1);
}

bool send_get_next_qr_code_bitmap(int8_t chunk)
{
    ExtraRequestKeyValuePair extras[] = {{.key = MESSAGE_KEY_chunk, .type = VALUE_TYPE_INT, .value.num = chunk}};

    return send_request_with_extra("get_next_qr_code_bitmap", extras, 1);
}

bool send_get_story_available_details(char *category_name, char *story_id)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_categoryName, .type = VALUE_TYPE_STRING, .value.str = category_name},
        {.key = MESSAGE_KEY_storyId, .type = VALUE_TYPE_STRING, .value.str = story_id}};

    return send_request_with_extra("get_available_details", extras, 2);
}

bool send_get_story_detail(char *story_detail)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_detailType, .type = VALUE_TYPE_STRING, .value.str = story_detail}};
    return send_request_with_extra("get_story_detail", extras, 1);
}

bool send_get_story_detail_sources(char *type, char *key)
{
    ExtraRequestKeyValuePair extras[] = {{.key = MESSAGE_KEY_detailType, .type = VALUE_TYPE_STRING, .value.str = type},
                                         {.key = MESSAGE_KEY_detailKey, .type = VALUE_TYPE_STRING, .value.str = key}};
    return send_request_with_extra("get_story_detail_sources", extras, 2);
}

bool send_get_current_detail_text(char *type, char *key)
{
    ExtraRequestKeyValuePair extras[] = {{.key = MESSAGE_KEY_detailType, .type = VALUE_TYPE_STRING, .value.str = type},
                                         {.key = MESSAGE_KEY_detailKey, .type = VALUE_TYPE_STRING, .value.str = key}};
    return send_request_with_extra("get_story_detail", extras, 2);
}

bool send_debug_log(int32_t level, const char *tag, const char *message)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_logLevel, .type = VALUE_TYPE_INT, .value.num = level},
        {.key = MESSAGE_KEY_logTag, .type = VALUE_TYPE_STRING, .value.str = tag},
        {.key = MESSAGE_KEY_logMessage, .type = VALUE_TYPE_STRING, .value.str = message}};
    return send_request_with_extra("debug_log", extras, 3);
}

bool send_debug_notify(const char *title, const char *message)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_logTag, .type = VALUE_TYPE_STRING, .value.str = title},
        {.key = MESSAGE_KEY_logMessage, .type = VALUE_TYPE_STRING, .value.str = message}};
    return send_request_with_extra("debug_notify", extras, 2);
}

bool send_mark_story_read(const char *story_id)
{
    ExtraRequestKeyValuePair extras[] = {
        {.key = MESSAGE_KEY_storyId, .type = VALUE_TYPE_STRING, .value.str = story_id}};
    return send_request_with_extra("mark_story_read", extras, 1);
}

void communication_init(void)
{
    message_router_init();

    message_router_register("update_categories", handle_update_categories_message);
    message_router_register("get_category_names", handle_get_category_names_message);
    message_router_register("get_story_titles", handle_get_story_titles_message);
    message_router_register("get_short_summary", handle_get_short_summary_message);
    message_router_register("get_qr_code_bitmap", handle_get_qr_code_bitmap_message);
    message_router_register("get_next_qr_code_bitmap", handle_get_qr_code_bitmap_message);
    message_router_register("get_available_details", handle_get_story_available_details_message);
    message_router_register("get_story_detail", handle_get_story_detail_message);
    message_router_register("get_story_detail_sources", handle_get_story_detail_sources_message);
    message_router_register("set_debug_mode", handle_set_debug_mode_message);
    message_router_register("send_interface_strings", handle_send_interface_strings_message);
    message_router_register("restart_app", handle_restart_app_message);
    message_router_register("set_text_size", handle_set_text_size_message);

    // Use the lesser of requested size and maximum allowed size.
    // However sizes smaller than requested will probably break things
    {
        uint32_t requested_inbox = APP_MESSAGE_INBOX_SIZE;
        uint32_t max_inbox = app_message_inbox_size_maximum();
        if (max_inbox < requested_inbox)
        {
            requested_inbox = max_inbox;
        }
        uint32_t requested_outbox = APP_MESSAGE_OUTBOX_SIZE;
        if (APP_MESSAGE_OUTBOX_SIZE_MAXIMUM < requested_outbox)
        {
            requested_outbox = APP_MESSAGE_OUTBOX_SIZE_MAXIMUM;
        }
        INFO_LOG("KNCCommunication", "Opening AppMessage inbox: %lu outbox: %lu (max inbox: %lu)",
                 (unsigned long)requested_inbox, (unsigned long)requested_outbox, (unsigned long)max_inbox);

        app_message_register_inbox_received(inbox_received_callback);
        app_message_register_outbox_sent(outbox_sent_callback);
        app_message_register_outbox_failed(outbox_failed_callback);
        app_message_open(requested_inbox, requested_outbox);
    }

    debug_logger_init();
}

void communication_deinit(void)
{
    debug_logger_deinit();
    message_router_deinit();
}
