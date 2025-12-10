#pragma once

#include <pebble.h>

typedef struct
{
    const char *title;                  // Menu title displayed in header
    uint16_t (*get_num_items)(void);    // Callback to get number of items
    char **(*get_items)(void);          // Callback to get array of items
    void (*select_callback)(int index); // Callback when item is selected
    bool (*is_item_read)(int index);    // Optional callback to check if item is read
} MenuConfig;

/**
 * Creates and shows a new menu window with the given configuration
 * without multiline support
 * @param config Menu configuration including callbacks and title
 * @return Pointer to the created Window
 */
Window *menu_handler_create_window_no_multiline(MenuConfig config);

/**
 * Creates and shows a new menu window with the given configuration
 * @param config Menu configuration including callbacks and title
 * @return Pointer to the created Window
 */
Window *menu_handler_create_window(MenuConfig config);

/**
 * Set the callback to check if an item is read
 * @param window The menu window
 * @param callback The callback function
 */
void menu_handler_set_is_read_callback(Window *window, bool (*callback)(int index));

/**
 * Destroy a menu window and clean up resources
 * @param window The window to destroy
 */
void menu_handler_destroy_window(Window *window);

/**
 * Request a menu data update, causing a redraw
 * @param window The window containing the menu to update
 */
void menu_handler_request_update(Window *window);