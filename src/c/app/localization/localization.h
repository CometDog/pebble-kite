#pragma once

#include "strings-def.h"
#include <pebble.h>

/**
 * Localization system for Kagi News app
 * Supports the following locales: en_US (default), fr_FR, de_DE, es_ES, it_IT, pt_PT
 */

// String identifiers for all translatable strings in the app

/**
 * Initialize localization system
 * Sets locale to system default or empty string for English
 */
void localization_init(void);

/**
 * Deinitialize localization system
 */
void localization_deinit(void);

/**
 * Provide a JSON string of translations coming from the phone.
 * The JSON should be an object containing key/value pairs where keys
 * correspond to the `StringId` names (for example "STRING_CATEGORIES").
 * Nested objects are supported (parser will find keys anywhere in the tree).
 * The C side will store these values and `localization_get_string` will
 * return them when present.
 */
void localization_set_strings(const char *strings_json);

/**
 * Get localized string by ID
 * @param string_id The string identifier
 * @return Localized string, or English fallback if locale not found
 */
const char *localization_get_string(StringId string_id);

/**
 * Translate a category name from English to the current locale
 * @param category_name The category name in English (e.g., "Business", "Gaming")
 * @return Localized category name, or the original name if no translation found
 */
const char *localization_translate_category(const char *category_name);
