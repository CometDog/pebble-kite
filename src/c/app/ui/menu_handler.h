#pragma once

#include <pebble.h>

typedef struct
{
    int section_index;
    const char *title;
    int num_items;
} SectionInfo;

typedef struct
{
    // TODO: This could get cleaned up but for now it's a mess to handle single and multi-section menus
    const char *title;                                                              // Menu title (used for single section menus)
    bool has_status_bar;                                                            // Whether to reserve space for a status bar at the top
    uint16_t (*get_num_sections)(void);                                             // Optional: number of sections (NULL for single section)
    uint16_t (*get_num_items)(void);                                                // Callback to get number of items (for single section)
    uint16_t (*get_num_items_in_section)(uint16_t section_index);                   // Callback for multi-section menus
    char **(*get_items)(void);                                                      // Callback to get array of items (for single section)
    char **(*get_items_in_section)(uint16_t section_index);                         // Callback for multi-section menus
    const char *(*get_section_title)(uint16_t section_index);                       // Callback to get section title
    void (*select_callback)(int index);                                             // Callback when item is selected (for single section)
    void (*select_callback_in_section)(uint16_t section_index, uint16_t row_index); // Callback for multi-section
    bool (*is_item_read)(int index);                                                // Optional callback to check if item is read (for single section)
    bool (*is_item_read_in_section)(uint16_t section_index, uint16_t row_index);    // Optional for multi-section
    void (*up_press_handler_from_top_of_list)(void);                                // Optional callback for up press on top item
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

/**
 * Update the menu title
 * @param window The window containing the menu
 * @param new_title The new title string
 */
void menu_handler_update_title(Window *window, const char *new_title);