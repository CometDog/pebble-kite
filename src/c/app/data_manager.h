#pragma once

#include <pebble.h>

typedef enum
{
    DATA_RESOURCE_PRIMARY_STORY_OPTIONS,
    DATA_RESOURCE_PRIMARY_STORY_TITLE,
    DATA_RESOURCE_PRIMARY_STORY_TEXT,
    DATA_RESOURCE_CATEGORIES,
    DATA_RESOURCE_STORY_DETAILS,
    DATA_RESOURCE_STORY_DETAIL_OPTIONS,
    DATA_RESOURCE_STORY_DETAIL_TITLE,
    DATA_RESOURCE_STORY_DETAIL_TEXT,
    DATA_RESOURCE_ACTIVE_DETAIL_SOURCE_OPTIONS,
    DATA_RESOURCE_QR_CODE,
} DataResource;

// Typed context describing common resource context fields. All pointers are
// non-owned when passed in; the manager will make owned copies.
typedef struct
{
    const char *category_name;
    const char *story_title;
    const char *story_id;
    const char *article_link;
} DataContext;

// Opaque handle representing a window registration. Returned by register and
// used to unregister later. Internally this is a Window* cast for simplicity.
typedef void *DataRegistrationHandle;

/**
 * Initialize the data manager
 */
void data_manager_init(void);
/**
 * Deinitialize the data manager, cleaning up resources
 */
void data_manager_deinit(void);
/**
 * Register window resource requirements with the data manager.
 * @param window The window that requires the resources
 * @param resources Array of DataResource enums representing required resources
 * @param resource_count Number of resources in the resources array
 * @return A DataRegistrationHandle that must be used to unregister later
 */
DataRegistrationHandle data_manager_register_window_requirements(Window *window, DataResource *resources,
                                                                 size_t resource_count);
/**
 * Unregister window resource requirements from the data manager.
 * @param handle The DataRegistrationHandle returned during registration
 */
void data_manager_unregister_window_requirements(DataRegistrationHandle handle);
/**
 * Request a specific data resource immediately.
 * @param resource The DataResource to request
 * @param context Optional DataContext providing additional context for the request
 */
void data_manager_request_resource(DataResource resource, const DataContext *context);
/**
 * Refresh data for a registered window (re-request all its resources)
 * @param handle The DataRegistrationHandle for the window
 */
void data_manager_refresh_window_data(DataRegistrationHandle handle);
