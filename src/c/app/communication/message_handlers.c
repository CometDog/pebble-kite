#include "message_handlers.h"
#include "../app.h"
#include "../communication.h"
#include "../data.h"
#include "../localization/localization.h"
#include "../ui/action_items/action_items_detail_list_window.h"
#include "../ui/action_items/action_items_detail_text_window.h"
#include "../ui/background/background_detail_text_window.h"
#include "../ui/categories.h"
#include "../ui/did_you_know/did_you_know_detail_text_window.h"
#include "../ui/humanitarian_impact/humanitarian_impact_detail_text_window.h"
#include "../ui/industry_impact/industry_impact_detail_list_window.h"
#include "../ui/industry_impact/industry_impact_detail_text_window.h"
#include "../ui/international_reactions/international_reactions_detail_list_window.h"
#include "../ui/international_reactions/international_reactions_detail_text_window.h"
#include "../ui/main_story/main_story_list_window.h"
#include "../ui/performance_statistics/performance_statistics_detail_list_window.h"
#include "../ui/performance_statistics/performance_statistics_detail_text_window.h"
#include "../ui/perspective/perspective_detail_list_window.h"
#include "../ui/perspective/perspective_detail_text_window.h"
#include "../ui/qna/qna_detail_list_window.h"
#include "../ui/qna/qna_detail_text_window.h"
#include "../ui/quote/quote_detail_text_window.h"
#include "../ui/scientific_significance/scientific_significance_detail_list_window.h"
#include "../ui/scientific_significance/scientific_significance_detail_text_window.h"
#include "../ui/source/active_detail_source_list_window.h"
#include "../ui/source/source_detail_list_window.h"
#include "../ui/talking_point/talking_point_detail_list_window.h"
#include "../ui/talking_point/talking_point_detail_text_window.h"
#include "../ui/technical_detail/technical_detail_detail_list_window.h"
#include "../ui/technical_detail/technical_detail_detail_text_window.h"
#include "../ui/timeline/timeline_detail_list_window.h"
#include "../ui/timeline/timeline_detail_text_window.h"
#include "../ui/travel_advisory/travel_advisory_detail_list_window.h"
#include "../ui/travel_advisory/travel_advisory_detail_text_window.h"
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
        Tuple *category_names_tuple = dict_find(iter, MESSAGE_KEY_data);
        if (!category_names_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Category names message missing data");
            return;
        }

        push_local_categories(category_names_tuple->value->cstring);

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
        Tuple *batch_id_tuple = dict_find(iter, MESSAGE_KEY_batchId);
        Tuple *category_name_tuple = dict_find(iter, MESSAGE_KEY_categoryName);
        Tuple *short_data_tuple = dict_find(iter, MESSAGE_KEY_shortData);
        Tuple *story_titles = dict_find(iter, MESSAGE_KEY_data1);
        Tuple *story_read_statuses = dict_find(iter, MESSAGE_KEY_data2);

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
        Tuple *full_title_tuple = dict_find(iter, MESSAGE_KEY_data1);
        Tuple *full_short_summary_tuple = dict_find(iter, MESSAGE_KEY_data2);
        Tuple *story_id_tuple = dict_find(iter, MESSAGE_KEY_data3);
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
        Tuple *bitmap_data_tuple = dict_find(iter, MESSAGE_KEY_data);
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
        Tuple *available_details_tuple = dict_find(iter, MESSAGE_KEY_data);
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
        Tuple *story_detail_type_tuple = dict_find(iter, MESSAGE_KEY_data1);
        Tuple *story_detail_tuple = dict_find(iter, MESSAGE_KEY_data2);
        Tuple *story_detail_extra_tuple = dict_find(iter, MESSAGE_KEY_data3);
        Tuple *story_detail_sources_tuple = dict_find(iter, MESSAGE_KEY_data4);
        if (!story_detail_type_tuple || !story_detail_tuple)
        {
            ERROR_LOG("KNCMessageHandling", "Story detail message missing data");
            return;
        }

        void (*set_detail)(char *data, void (*ui_update_callback)(void)) = NULL;
        void (*ui_update_callback)(void) = NULL;
        if (strcmp(story_detail_type_tuple->value->cstring, "Background") == 0)
        {
            set_detail = set_detail_text;
            ui_update_callback = background_detail_text_window_ui_update;
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "Articles") == 0)
        {
            set_detail = set_detail_options;
            ui_update_callback = source_detail_list_window_ui_update;
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "Perspectives") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "povs") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = perspective_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = perspective_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Perspective detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for PerspectivePovs detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "TalkingPoints") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "povs") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = talking_point_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = talking_point_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Talking Points detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Talking Points detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "TechnicalDetails") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "povs") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = technical_detail_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = technical_detail_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Technical Details detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Technical Details detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "IndustryImpact") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "povs") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = industry_impact_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = industry_impact_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Industry Impact detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Industry Impact detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "QnA") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "qnas") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = qna_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = qna_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown QnA detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for QnA detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "Timeline") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "dates") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = timeline_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = timeline_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Timeline detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Timeline detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "HumanitarianImpact") == 0)
        {
            set_detail = set_detail_text;
            ui_update_callback = humanitarian_impact_detail_text_window_ui_update;
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "TravelAdvisory") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "advisories") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = travel_advisory_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = travel_advisory_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Travel Advisory detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Travel Advisory detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "DidYouKnow") == 0)
        {
            set_detail = set_detail_text;
            ui_update_callback = did_you_know_detail_text_window_ui_update;
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "Quote") == 0)
        {
            // For Quote, data3 contains the author which is used as the title
            if (story_detail_extra_tuple && story_detail_extra_tuple->value->cstring &&
                strlen(story_detail_extra_tuple->value->cstring) > 0)
            {
                set_detail_title(story_detail_extra_tuple->value->cstring, NULL);
            }
            else
            {
                set_detail_title("Quote", NULL);
            }
            set_detail = set_detail_text;
            ui_update_callback = quote_detail_text_window_ui_update;
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "InternationalReactions") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "reactions") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = international_reactions_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = international_reactions_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown International Reactions detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for International Reactions detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "UserActionItems") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "action_items") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = action_items_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = action_items_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Action Items detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Action Items detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "PerformanceStatistics") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "statistics") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = performance_statistics_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = performance_statistics_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Performance Statistics detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Performance Statistics detail");
            }
        }
        else if (strcmp(story_detail_type_tuple->value->cstring, "ScientificSignificance") == 0)
        {
            if (story_detail_extra_tuple && story_detail_tuple)
            {
                if (strcmp(story_detail_extra_tuple->value->cstring, "significance") == 0)
                {
                    set_detail = set_detail_options;
                    ui_update_callback = scientific_significance_detail_list_window_ui_update;
                }
                else if (strcmp(story_detail_extra_tuple->value->cstring, "text") == 0)
                {
                    set_detail = set_detail_text;
                    ui_update_callback = scientific_significance_detail_text_window_ui_update;
                }
                else
                {
                    ERROR_LOG("KNCMessageHandling", "Unknown Scientific Significance detail key: %s",
                              story_detail_extra_tuple->value->cstring);
                }
            }
            else
            {
                ERROR_LOG("KNCMessageHandling", "Missing data for Scientific Significance detail");
            }
        }
        else
        {
            ERROR_LOG("KNCMessageHandling", "Unknown story detail type: %s", story_detail_type_tuple->value->cstring);
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
        Tuple *story_detail_sources_tuple = dict_find(iter, MESSAGE_KEY_data);
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
    Tuple *enabled_tuple = dict_find(iter, MESSAGE_KEY_data);
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
    Tuple *strings_tuple = dict_find(iter, MESSAGE_KEY_data);
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
    Tuple *text_size_tuple = dict_find(iter, MESSAGE_KEY_data);
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
