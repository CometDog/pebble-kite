#include "message_handlers.h"
#include "../app.h"
#include "../communication.h"
#include "../data.h"
#include "../detail_types.h"
#include "../localization/localization.h"
#include "../ui/categories.h"
#include "../ui/detail_window_registry.h"
#include "../ui/main_story/main_story_list_window.h"
#include "../ui/source/active_detail_source_list_window.h"
#include "../ui/source/source_detail_list_window.h"
#include "../ui/ui_config.h"
#include "../utils/debug_logger.h"

void handle_update_categories_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Update categories message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        clear_categories_data();
        send_get_category_names(1);
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Update categories failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_category_names_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get category names message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *category_names_tuple = dict_find(iter, MESSAGE_KEY_categoryNamePage);
        if (!category_names_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Category names message missing data");
            return;
        }
        push_local_categories(category_names_tuple->value->cstring);

        Tuple *additional_feeds_count_tuple = dict_find(iter, MESSAGE_KEY_additionalFeedsCount);
        if (additional_feeds_count_tuple)
        {
            set_additional_feeds_count(additional_feeds_count_tuple->value->int8);
        }

        Tuple *next_page_tuple = dict_find(iter, MESSAGE_KEY_nextPage);
        if (next_page_tuple)
        {
            send_get_category_names(next_page_tuple->value->int8);
        }
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get category names failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_story_titles_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get story titles message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *category_name_tuple = dict_find(iter, MESSAGE_KEY_categoryName);
        Tuple *short_data_tuple = dict_find(iter, MESSAGE_KEY_shortData);
        Tuple *story_titles = dict_find(iter, MESSAGE_KEY_storyTitles);
        Tuple *story_read_statuses = dict_find(iter, MESSAGE_KEY_storyReadStatuses);

        if (!story_titles || !category_name_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Story titles message missing required data");
            return;
        }

        push_story_titles(story_titles->value->cstring);

        if (story_read_statuses)
        {
            push_story_read_statuses(story_read_statuses->value->cstring);
        }

        Tuple *next_page_tuple = dict_find(iter, MESSAGE_KEY_nextPage);
        if (next_page_tuple && short_data_tuple)
        {
            send_get_story_titles(next_page_tuple->value->int8, category_name_tuple->value->cstring,
                                  short_data_tuple->value->int8 == 1 ? true : false);
        }
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get story titles failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_short_summary_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get full data message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *full_title_tuple = dict_find(iter, MESSAGE_KEY_storyTitle);
        Tuple *full_short_summary_tuple = dict_find(iter, MESSAGE_KEY_storySummary);
        Tuple *story_id_tuple = dict_find(iter, MESSAGE_KEY_storyId);
        if (!full_title_tuple || !full_short_summary_tuple || !story_id_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Full data message missing title, short summary, or story ID");
            return;
        }
        set_story_full_title(full_title_tuple->value->cstring);
        set_story_short_summary(full_short_summary_tuple->value->cstring);
        set_story_id(story_id_tuple->value->cstring);
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get full data failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_qr_code_bitmap_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get QR code bitmap message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *bitmap_data_tuple = dict_find(iter, MESSAGE_KEY_qrData);
        Tuple *next_chunk_tuple = dict_find(iter, MESSAGE_KEY_nextChunk);
        Tuple *qr_size_tuple = dict_find(iter, MESSAGE_KEY_qrSize);
        if (!bitmap_data_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "QR code bitmap message missing data");
            return;
        }

        uint16_t chunk_length = bitmap_data_tuple->length;

        if (next_chunk_tuple && next_chunk_tuple->value->int8 == 1)
        {
            clear_qr_code_data();
            // Set QR size from first chunk response
            if (qr_size_tuple)
            {
                set_qr_size((uint8_t)qr_size_tuple->value->int32);
            }
        }

        if (chunk_length > 0)
        {
            push_qr_code_chunk((uint8_t *)bitmap_data_tuple->value->data, chunk_length);
        }

        if (next_chunk_tuple)
        {
            send_get_next_qr_code_bitmap(next_chunk_tuple->value->int8);
        }
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get QR code bitmap failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_story_available_details_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get available details message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *available_details_tuple = dict_find(iter, MESSAGE_KEY_availableDetailTypes);
        if (!available_details_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Available details message missing data");
            return;
        }

        set_available_details(available_details_tuple->value->cstring);
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get available details failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_story_detail_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get story detail message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *story_detail_type_tuple = dict_find(iter, MESSAGE_KEY_detailType);
        Tuple *story_detail_tuple = dict_find(iter, MESSAGE_KEY_detailContent);
        Tuple *story_detail_extra_tuple = dict_find(iter, MESSAGE_KEY_detailContentType);
        Tuple *story_detail_sources_tuple = dict_find(iter, MESSAGE_KEY_detailHasSources);
        if (!story_detail_type_tuple || !story_detail_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Story detail message missing data");
            return;
        }

        const char *detail_type_name = story_detail_type_tuple->value->cstring;
        const DetailTypeInfo *info = detail_type_from_api_name(detail_type_name);

        if (!info)
        {
            ERROR_LOG("KNCMessageHandling", "Unknown story detail type: %s", detail_type_name);
            return;
        }

        void (*set_detail)(char *data, void (*ui_update_callback)(void)) = NULL;
        void (*ui_update_callback)(void) = NULL;

        // Special handling for Quote title (author)
        if (info->detail_type == DETAIL_TYPE_QUOTE && story_detail_extra_tuple)
        {
            if (story_detail_extra_tuple->value->cstring && strlen(story_detail_extra_tuple->value->cstring) > 0)
            {
                set_detail_title(story_detail_extra_tuple->value->cstring, NULL);
            }
            else
            {
                set_detail_title("Quote", NULL);
            }
        }

        // Special handling for Articles (sources) which uses a different window system
        if (info->detail_type == DETAIL_TYPE_ARTICLES)
        {
            set_detail = set_detail_options;
            ui_update_callback = source_detail_list_window_ui_update;
        }
        // For all other types, use the common registry
        else if (story_detail_extra_tuple)
        {
            const char *data_type = story_detail_extra_tuple->value->cstring;

            // Check if this is list data (povs, qnas, dates, advisories, etc.) or text data
            if (strcmp(data_type, "text") == 0)
            {
                set_detail = set_detail_text;
                ui_update_callback = detail_window_registry_update_text;
            }
            else if (info->has_list)
            {
                // Any other data type is list data
                set_detail = set_detail_options;
                ui_update_callback = detail_window_registry_update_list;
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Unknown detail data type: %s for %s", data_type, detail_type_name);
            }
        }
        // Text-only types (no preceding list data)
        else if (info->has_text && !info->has_list)
        {
            set_detail = set_detail_text;
            ui_update_callback = detail_window_registry_update_text;
        }

        if (set_detail && ui_update_callback)
        {
            set_detail(story_detail_tuple->value->cstring, ui_update_callback);
        }

        if (story_detail_sources_tuple)
        {
            set_detail_has_sources(story_detail_sources_tuple->value->int8 == 1 ? true : false, ui_update_callback);
        }
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get story detail failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_get_story_detail_sources_message(DictionaryIterator *iter)
{
    Tuple *state_tuple = dict_find(iter, MESSAGE_KEY_state);
    if (!state_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Get story detail sources message missing state");
        return;
    }

    if (strcmp(state_tuple->value->cstring, "success") == 0)
    {
        Tuple *story_detail_sources_tuple = dict_find(iter, MESSAGE_KEY_articleDomains);
        if (!story_detail_sources_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Story detail sources message missing data");
            return;
        }

        set_detail_sources(story_detail_sources_tuple->value->cstring, active_detail_source_list_window_ui_update);
    }
    else
    {
        ERROR_LOG("KNCMessageHandling", "Get story detail sources failed with state: %s", state_tuple->value->cstring);
    }
}

void handle_set_debug_mode_message(DictionaryIterator *iter)
{
    Tuple *enabled_tuple = dict_find(iter, MESSAGE_KEY_isDebugMode);
    if (!enabled_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Set debug mode message missing data");
        return;
    }

    bool enabled = enabled_tuple->value->int32 != 0;
    debug_logger_set_enabled(enabled);
}

void handle_restart_app_message(DictionaryIterator *iter)
{
    INFO_LOG("KNCMessageHandling", "Received restart_app message from phone, restarting app");
    app_restart();
}

void handle_send_interface_strings_message(DictionaryIterator *iter)
{
    Tuple *strings_tuple = dict_find(iter, MESSAGE_KEY_interfaceStrings);
    if (!strings_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Send interface strings message missing data");
        return;
    }

    const char *strings = strings_tuple->value->cstring;
    localization_set_strings(strings);
    INFO_LOG("KNCMessageHandling", "Interface strings changed, restarting app");
}

void handle_set_text_size_message(DictionaryIterator *iter)
{
    Tuple *text_size_tuple = dict_find(iter, MESSAGE_KEY_textSize);
    if (!text_size_tuple)
    {
        ERROR_LOG("KNCMessageHandling", "Set text size message missing data");
        return;
    }

    const char *text_size = text_size_tuple->value->cstring;
    INFO_LOG("KNCMessageHandling", "Received set_text_size message from phone, text size: %s", text_size);
    ui_set_text_size_override(atoi(text_size));

    ui_reload_fonts();
}
