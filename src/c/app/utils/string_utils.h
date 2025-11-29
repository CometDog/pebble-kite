#pragma once

#include <pebble.h>

/**
 * Creates a new copy of the input string.
 * @param src The source string to duplicate
 * @return A newly allocated string containing a copy of src, or NULL if memory allocation fails
 */
char *string_duplicate(const char *src);
/**
 * Trims whitespace from both ends of a string in-place.
 * @param str The string to trim
 * @return The same string pointer, now trimmed
 */
char *string_trim(char *str);
/**
 * Splits a string by a delimiter and stores results in an array.
 * @param array The array to store split strings in
 * @param current_count Current number of items in the array
 * @param max_capacity Maximum capacity of the array
 * @param string String to split
 * @param delimiter Delimiter to split on
 * @return Number of new items added to the array
 */
int8_t string_split_to_array(char **array, int8_t current_count, int8_t max_capacity, char *string,
                             const char *delimiter);