#pragma once

#include <pebble.h>

/**
 * Pagination helpers for list screens.
 *
 * Sections and logical pages in list windows map a contiguous array of items
 * into multiple display sections. These helpers centralize the logic so
 * each screen doesn't duplicate it.
 *
 * Terminology used throughout:
 *   page         – 0-based page/section index
 *   items        – total number of items across ALL sections
 *   section_size – maximum number of items in a single section
 */

/**
 * Returns the first item index (0-based, inclusive) that belongs to the
 * given section.
 *
 * @param section      0-based section index
 * @param section_size Number of items per section
 */
uint16_t page_start(uint16_t section, uint16_t section_size);

/**
 * Returns one past the last item index that belongs to the given section,
 * clamped to `total`.
 *
 * @param section      0-based section index
 * @param section_size Number of items per section
 * @param total        Total number of items
 */
uint16_t page_end(uint16_t section, uint16_t section_size, uint16_t total);

/**
 * Returns the number of items that belong to the given section.
 *
 * @param section      0-based section index
 * @param section_size Number of items per section
 * @param total        Total number of items
 */
uint16_t page_count(uint16_t section, uint16_t section_size, uint16_t total);

/**
 * Returns true if there is a section after this one.
 *
 * @param section      0-based section index
 * @param section_size Number of items per section
 * @param total        Total number of items
 */
bool has_next_section(uint16_t section, uint16_t section_size, uint16_t total);

/**
 * Returns true if there is a section before this one.
 *
 * @param section 0-based section index
 */
bool has_prev_section(uint16_t section);
