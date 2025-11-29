#include "data_manager.h"
#include "communication.h"
#include "data.h"
#include "ui/navigation.h"
#include "utils/string_utils.h"
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTRATIONS 16
#define MAX_RESOURCES ((int)DATA_RESOURCE_QR_CODE + 1)

typedef struct
{
    Window *window;
    DataResource *resources;
    size_t resource_count;
    DataContext *context;
    NavigationTier tier;
} Registration;

static Registration s_registrations[MAX_REGISTRATIONS];
static size_t s_registration_count = 0;

static int s_refcounts[MAX_RESOURCES];

void data_manager_init(void)
{
    memset(s_registrations, 0, sizeof(s_registrations));
    s_registration_count = 0;
    memset(s_refcounts, 0, sizeof(s_refcounts));
}

void data_manager_deinit(void)
{
    for (size_t i = 0; i < s_registration_count; ++i)
    {
        for (size_t r = 0; r < s_registrations[i].resource_count; ++r)
        {
            DataResource res = s_registrations[i].resources[r];
            int idx = (int)res;
            if (idx >= 0 && idx < MAX_RESOURCES)
            {
                s_refcounts[idx] = 0;
            }
        }
        if (s_registrations[i].resources)
        {
            free(s_registrations[i].resources);
            s_registrations[i].resources = NULL;
        }
        if (s_registrations[i].context)
        {
            if (s_registrations[i].context->category_name)
                free((void *)s_registrations[i].context->category_name);
            if (s_registrations[i].context->story_title)
                free((void *)s_registrations[i].context->story_title);
            if (s_registrations[i].context->story_id)
                free((void *)s_registrations[i].context->story_id);
            if (s_registrations[i].context->article_link)
                free((void *)s_registrations[i].context->article_link);
            free(s_registrations[i].context);
            s_registrations[i].context = NULL;
        }
    }
    s_registration_count = 0;
}

static void request_resource_internal(DataResource resource, const DataContext *context)
{
    switch (resource)
    {
    case DATA_RESOURCE_CATEGORIES:
        // Do nothing here, rely on communication during init to get categories
        break;
    case DATA_RESOURCE_PRIMARY_STORY_OPTIONS:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_PRIMARY_STORY_TITLE:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_PRIMARY_STORY_TEXT:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_STORY_DETAILS:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_STORY_DETAIL_OPTIONS:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_ACTIVE_DETAIL_SOURCE_OPTIONS:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_STORY_DETAIL_TITLE:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_STORY_DETAIL_TEXT:
        // UNIMPLEMENTED
        break;
    case DATA_RESOURCE_QR_CODE:
        // UNIMPLEMENTED
        break;
    default:
        break;
    }
}

DataRegistrationHandle data_manager_register_window_requirements(Window *window, DataResource *resources,
                                                                 size_t resource_count)
{
    if (!window || !resources || resource_count == 0)
        return NULL;

    if (s_registration_count >= MAX_REGISTRATIONS)
        return NULL;

    // Copy resources array so caller can free
    DataResource *res_copy = malloc(sizeof(DataResource) * resource_count);
    if (!res_copy)
        return NULL;
    memcpy(res_copy, resources, sizeof(DataResource) * resource_count);

    // Deep-copy the context so manager owns it and callers don't need to keep it alive
    DataContext *context = NULL;

    // Store the current navigation tier with this registration
    NavigationTier current_tier = navigation_get_current_tier();

    Registration reg = {.window = window,
                        .resources = res_copy,
                        .resource_count = resource_count,
                        .context = context,
                        .tier = current_tier};
    s_registrations[s_registration_count] = reg;
    s_registration_count++;

    // Increment refcounts and request resources if this window is top-most
    for (size_t i = 0; i < resource_count; ++i)
    {
        DataResource res = res_copy[i];
        int idx = (int)res;
        if (idx >= 0 && idx < MAX_RESOURCES)
        {
            s_refcounts[idx]++;
            if (s_refcounts[idx] == 1)
            {
                request_resource_internal(res, context);
            }
        }
    }

    return (DataRegistrationHandle)window;
}

void data_manager_unregister_window_requirements(DataRegistrationHandle handle)
{
    if (!handle)
        return;

    Window *window = (Window *)handle;

    for (size_t i = 0; i < s_registration_count; ++i)
    {
        if (s_registrations[i].window == window)
        {
            for (size_t r = 0; r < s_registrations[i].resource_count; ++r)
            {
                DataResource res = s_registrations[i].resources[r];
                int idx = (int)res;
                if (idx >= 0 && idx < MAX_RESOURCES)
                {
                    if (s_refcounts[idx] > 0)
                        s_refcounts[idx]--;
                }
            }

            // Clear data hierarchically based on the tier of the window being unregistered
            // When a window at a given tier unloads, we clear data at that tier and all deeper tiers
            NavigationTier window_tier = s_registrations[i].tier;

            // Clear data at this tier and all deeper tiers
            // NOTE: Categories are NEVER cleared during navigation, only on app deinit
            switch (window_tier)
            {
            case NAV_TIER_1_CATEGORIES:
                clear_story_list_data();
                clear_story_data();
                clear_available_details_data();
                clear_detail_list_data();
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_2_STORY_TITLES:
                clear_story_list_data();
                clear_story_data();
                clear_available_details_data();
                clear_detail_list_data();
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_2_STORY_SUMMARY:
                clear_story_data();
                clear_available_details_data();
                clear_detail_list_data();
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_3_AVAILABLE_DETAILS:
                clear_available_details_data();
                clear_detail_list_data();
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_4_DETAIL_LIST:
                clear_detail_list_data();
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;
            case NAV_TIER_4_DETAIL_TEXT:
                clear_detail_text_data();
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_5_SOURCES_LIST:
                clear_detail_sources_data();
                clear_qr_code_data();
                break;

            case NAV_TIER_6_QR_CODE:
                clear_qr_code_data();
                break;
            }

            if (s_registrations[i].resources)
            {
                free(s_registrations[i].resources);
                s_registrations[i].resources = NULL;
            }
            if (s_registrations[i].context)
            {
                if (s_registrations[i].context->category_name)
                    free((void *)s_registrations[i].context->category_name);
                if (s_registrations[i].context->story_title)
                    free((void *)s_registrations[i].context->story_title);
                if (s_registrations[i].context->story_id)
                    free((void *)s_registrations[i].context->story_id);
                if (s_registrations[i].context->article_link)
                    free((void *)s_registrations[i].context->article_link);
                free(s_registrations[i].context);
                s_registrations[i].context = NULL;
            }

            for (size_t j = i; j + 1 < s_registration_count; ++j)
            {
                s_registrations[j] = s_registrations[j + 1];
            }
            s_registration_count--;
            return;
        }
    }
}

void data_manager_request_resource(DataResource resource, const DataContext *context)
{
    request_resource_internal(resource, context);
}

void data_manager_refresh_window_data(DataRegistrationHandle handle)
{
    if (!handle)
        return;

    Window *window = (Window *)handle;

    // Find the registration for this window
    for (size_t i = 0; i < s_registration_count; ++i)
    {
        if (s_registrations[i].window == window)
        {
            // Re-request all resources for this window
            for (size_t r = 0; r < s_registrations[i].resource_count; ++r)
            {
                DataResource res = s_registrations[i].resources[r];
                request_resource_internal(res, s_registrations[i].context);
            }
            return;
        }
    }
}
