#pragma once

#include <pebble.h>

/**
 * Initialize the story Q&A UI
 */
void qna_detail_list_window_ui_init(void);
/**
 * Deinitialize the story Q&A UI
 */
void qna_detail_list_window_ui_deinit(void);
/**
 * Request an update to the available Q&A menu, causing a redraw
 */
void qna_detail_list_window_ui_update(void);
