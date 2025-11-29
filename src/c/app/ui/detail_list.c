#include "detail_list.h"
#include "menu_handler.h"
#include <stdlib.h>

Window *detail_list_create(Window **window, const char *title, uint16_t (*get_count)(void), char **(*get_items)(void),
                           void (*select_callback)(int))
{
    if (!window)
        return NULL;
    MenuConfig config = {
        .title = title, .get_num_items = get_count, .get_items = get_items, .select_callback = select_callback};

    Window *list = menu_handler_create_window(config);
    if (!list)
        return NULL;
    *window = list;
    return list;
}

Window *detail_list_create_with_resources(Window **window, const char *title, uint16_t (*get_count)(void),
                                          char **(*get_items)(void), void (*select_callback)(int),
                                          DataResource *resources, size_t resource_count,
                                          DataRegistrationHandle *handle)
{
    Window *list = detail_list_create(window, title, get_count, get_items, select_callback);
    if (!list)
        return NULL;
    if (resources && resource_count > 0 && handle)
    {
        *handle = data_manager_register_window_requirements(list, resources, resource_count);
    }
    return list;
}
