#pragma once

#include <pebble.h>

/**
 * Initialize and show the Q&A detail UI
 */
void qna_detail_text_window_ui_init(void);
/**
 * Deinitialize the Q&A detail UI
 */
void qna_detail_text_window_ui_deinit(void);
/**
 * Update the Q&A detail UI if visible
 */
void qna_detail_text_window_ui_update(void);
