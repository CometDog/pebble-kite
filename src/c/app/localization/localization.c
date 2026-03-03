#include "localization.h"
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

// Dynamic translations (populated by localization_set_strings)
static char *s_dynamic_strings[STRING_COUNT] = {0};

// Keys corresponding to each StringId in the same order as the enum
static const char *s_string_keys[STRING_COUNT] = {
    // general
    "STRING_LOADING",
    "STRING_TENSION_INDEX",
    "STRING_TENSION_INDEX_SHORT",
    "STRING_TENSION_INDEX_EXPLANATION_TITLE",
    "STRING_TENSION_INDEX_LEVEL_1",
    "STRING_TENSION_INDEX_LEVEL_2",
    "STRING_TENSION_INDEX_LEVEL_3",
    "STRING_TENSION_INDEX_LEVEL_4",
    "STRING_TENSION_INDEX_LEVEL_5",
    "STRING_TENSION_INDEX_EXPLANATION",
    // menus
    "STRING_FEEDS",
    "STRING_NEWS",
    "STRING_CATEGORIES",
    "STRING_STORIES",
    "STRING_AVAILABLE_DETAILS",
    "STRING_HISTORICAL_BACKGROUND",
    "STRING_HUMANITARIAN_IMPACT",
    "STRING_DID_YOU_KNOW",
    "STRING_QUOTE",
    "STRING_HIGHLIGHTS",
    "STRING_PERSPECTIVES",
    "STRING_QUICK_QUESTIONS",
    "STRING_TECHNICAL_DETAILS",
    "STRING_INDUSTRY_IMPACT",
    "STRING_TIMELINE",
    "STRING_TRAVEL_ADVISORY",
    "STRING_INTERNATIONAL_REACTIONS",
    "STRING_ACTION_ITEMS",
    "STRING_SCIENTIFIC_SIGNIFICANCE",
    "STRING_PERFORMANCE_STATISTICS",
    "STRING_SOURCES",
    // Generic category slots (up to 20)
    "STRING_CATEGORY_A",
    "STRING_CATEGORY_B",
    "STRING_CATEGORY_C",
    "STRING_CATEGORY_D",
    "STRING_CATEGORY_E",
    "STRING_CATEGORY_F",
    "STRING_CATEGORY_G",
    "STRING_CATEGORY_H",
    "STRING_CATEGORY_I",
    "STRING_CATEGORY_J",
    "STRING_CATEGORY_K",
    "STRING_CATEGORY_L",
    "STRING_CATEGORY_M",
    "STRING_CATEGORY_N",
    "STRING_CATEGORY_O",
    "STRING_CATEGORY_P",
    "STRING_CATEGORY_Q",
    "STRING_CATEGORY_R",
    "STRING_CATEGORY_S",
    "STRING_CATEGORY_T",
};

static void free_dynamic_strings(void)
{
    for (int i = 0; i < STRING_COUNT; ++i)
    {
        if (s_dynamic_strings[i])
        {
            free(s_dynamic_strings[i]);
            s_dynamic_strings[i] = NULL;
        }
    }
}

// Helper: uppercase a copy of key, replace non-alnum with '_' and prefix STRING_ if missing
static void build_alternate_key(const char *in, char *out, size_t out_len)
{
    size_t oi = 0;
    const char *p = in;
    // If already starts with STRING_, just uppercase and copy
    if (strncmp(in, "STRING_", 7) == 0)
    {
        while (*p && oi + 1 < out_len)
        {
            out[oi++] = toupper((unsigned char)*p);
            p++;
        }
        out[oi] = '\0';
        return;
    }

    // Start with prefix
    const char *prefix = "STRING_";
    while (*prefix && oi + 1 < out_len)
        out[oi++] = *prefix++;

    // Copy input, uppercase, replace non-alnum with '_'
    while (*p && oi + 1 < out_len)
    {
        char c = *p++;
        if (isalnum((unsigned char)c))
            out[oi++] = toupper((unsigned char)c);
        else
            out[oi++] = '_';
    }
    out[oi] = '\0';
}

// Find index for a key name. Accepts exact matches and alternate forms.
static int find_string_index_for_key(const char *key)
{
    if (!key || key[0] == '\0')
        return -1;

    // Try exact match
    for (int i = 0; i < STRING_COUNT; ++i)
    {
        if (strcmp(s_string_keys[i], key) == 0)
            return i;
    }

    // Try alternate (STRING_ + uppercase sanitized)
    char alt[128];
    build_alternate_key(key, alt, sizeof(alt));
    for (int i = 0; i < STRING_COUNT; ++i)
    {
        if (strcmp(s_string_keys[i], alt) == 0)
            return i;
    }

    return -1;
}

// Unescape a JSON string value into a newly allocated C string.
// Handles: \", \\, \n, \t, \r
static char *unescape_json_string(const char *src, size_t len)
{
    char *out = malloc(len + 1);
    if (!out)
        return NULL;
    size_t oi = 0;
    size_t i = 0;
    while (i < len)
    {
        char c = src[i++];
        if (c == '\\' && i < len)
        {
            char esc = src[i++];
            switch (esc)
            {
            case '"':
                out[oi++] = '"';
                break;
            case '\\':
                out[oi++] = '\\';
                break;
            case 'n':
                out[oi++] = '\n';
                break;
            case 't':
                out[oi++] = '\t';
                break;
            case 'r':
                out[oi++] = '\r';
                break;
            default:
                // Unknown escape, copy literally
                out[oi++] = esc;
                break;
            }
        }
        else
        {
            out[oi++] = c;
        }
    }
    out[oi] = '\0';
    return out;
}

// Explicit mapping for dotted keys (e.g. "title.category", "section.historicalBackground", "category.ai")
// to the corresponding StringId values defined in strings-def.h / localization.h
typedef struct
{
    const char *key;
    StringId id;
} KeyMap;

static const KeyMap s_dotted_map[] = {
    /* general */
    {"general.loading", STRING_LOADING},
    {"general.tensionIndex", STRING_TENSION_INDEX},
    {"general.tensionIndexShort", STRING_TENSION_INDEX_SHORT},
    {"general.tensionIndexExplanationTitle", STRING_TENSION_INDEX_EXPLANATION_TITLE},
    {"general.tensionIndexLevel1", STRING_TENSION_INDEX_LEVEL_1},
    {"general.tensionIndexLevel2", STRING_TENSION_INDEX_LEVEL_2},
    {"general.tensionIndexLevel3", STRING_TENSION_INDEX_LEVEL_3},
    {"general.tensionIndexLevel4", STRING_TENSION_INDEX_LEVEL_4},
    {"general.tensionIndexLevel5", STRING_TENSION_INDEX_LEVEL_5},
    {"general.tensionIndexExplanation", STRING_TENSION_INDEX_EXPLANATION},

    /* title */
    {"title.feeds", STRING_FEEDS},
    {"title.news", STRING_NEWS},
    {"title.category", STRING_CATEGORIES},
    {"title.stories", STRING_STORIES},
    {"title.availableDetails", STRING_AVAILABLE_DETAILS},

    /* sections */
    {"section.historicalBackground", STRING_HISTORICAL_BACKGROUND},
    {"section.humanitarianImpact", STRING_HUMANITARIAN_IMPACT},
    {"section.didYouKnow", STRING_DID_YOU_KNOW},
    {"section.quote", STRING_QUOTE},
    {"section.highlights", STRING_HIGHLIGHTS},
    {"section.perspectives", STRING_PERSPECTIVES},
    {"section.quickQuestions", STRING_QUICK_QUESTIONS},
    {"section.technicalDetails", STRING_TECHNICAL_DETAILS},
    {"section.industryImpact", STRING_INDUSTRY_IMPACT},
    {"section.timeline", STRING_TIMELINE},
    {"section.travelAdvisory", STRING_TRAVEL_ADVISORY},
    {"section.internationalReactions", STRING_INTERNATIONAL_REACTIONS},
    {"section.actionItems", STRING_ACTION_ITEMS},
    {"section.scientificSignificance", STRING_SCIENTIFIC_SIGNIFICANCE},
    {"section.performanceStatistics", STRING_PERFORMANCE_STATISTICS},
    {"section.sources", STRING_SOURCES},

    /* generic category slots (up to 20) */
    {"category.a", STRING_CATEGORY_A},
    {"category.b", STRING_CATEGORY_B},
    {"category.c", STRING_CATEGORY_C},
    {"category.d", STRING_CATEGORY_D},
    {"category.e", STRING_CATEGORY_E},
    {"category.f", STRING_CATEGORY_F},
    {"category.g", STRING_CATEGORY_G},
    {"category.h", STRING_CATEGORY_H},
    {"category.i", STRING_CATEGORY_I},
    {"category.j", STRING_CATEGORY_J},
    {"category.k", STRING_CATEGORY_K},
    {"category.l", STRING_CATEGORY_L},
    {"category.m", STRING_CATEGORY_M},
    {"category.n", STRING_CATEGORY_N},
    {"category.o", STRING_CATEGORY_O},
    {"category.p", STRING_CATEGORY_P},
    {"category.q", STRING_CATEGORY_Q},
    {"category.r", STRING_CATEGORY_R},
    {"category.s", STRING_CATEGORY_S},
    {"category.t", STRING_CATEGORY_T},
};

static int lookup_dotted_key(const char *key)
{
    if (!key)
        return -1;
    for (size_t i = 0; i < sizeof(s_dotted_map) / sizeof(s_dotted_map[0]); ++i)
    {
        if (strcmp(s_dotted_map[i].key, key) == 0)
            return (int)s_dotted_map[i].id;
    }
    return -1;
}

static const char **s_current_strings = NULL;

void localization_init(void)
{
    s_current_strings = NULL;
}

void localization_deinit(void)
{
    // Free any dynamic strings supplied from phone
    free_dynamic_strings();
}

void localization_set_strings(const char *strings)
{
    if (!strings || strlen(strings) == 0)
        return;

    // Free previous dynamic strings
    free_dynamic_strings();

    // If the incoming payload is a JSON-encoded string (double-stringified from JS),
    // it will look like: "{\"key\":\"value\",...}". Detect that and unescape
    // the outer string so we parse the inner JSON correctly.
    char *unquoted = NULL;
    const char *s = strings;
    size_t orig_len = strlen(strings);
    if (orig_len >= 2 && strings[0] == '"' && strings[orig_len - 1] == '"')
    {
        // unescape content between the quotes
        unquoted = unescape_json_string(strings + 1, orig_len - 2);
        if (unquoted)
        {
            s = unquoted;
        }
    }
    size_t len = strlen(s);
    size_t i = 0;

    while (i < len)
    {
        // Find next quoted token
        if (s[i] == '"')
        {
            // parse key candidate
            size_t kstart = i + 1;
            size_t k = kstart;
            while (k < len)
            {
                if (s[k] == '\\')
                {
                    k += 2;
                    continue;
                }
                if (s[k] == '"')
                    break;
                k++;
            }
            if (k >= len)
                break;
            size_t kend = k;
            // after closing quote
            size_t p = kend + 1;
            // skip spaces
            while (p < len && isspace((unsigned char)s[p]))
                p++;
            // expect ':' for a key
            if (p < len && s[p] == ':')
            {
                // Extract key string
                size_t keylen = kend - kstart;
                char keybuf[128];
                size_t copylen = keylen < (sizeof(keybuf) - 1) ? keylen : (sizeof(keybuf) - 1);
                memcpy(keybuf, &s[kstart], copylen);
                keybuf[copylen] = '\0';

                // Move p to value start
                p++;
                while (p < len && isspace((unsigned char)s[p]))
                    p++;

                // If value is a string
                if (p < len && s[p] == '"')
                {
                    size_t vstart = p + 1;
                    size_t v = vstart;
                    while (v < len)
                    {
                        if (s[v] == '\\')
                        {
                            v += 2;
                            continue;
                        }
                        if (s[v] == '"')
                            break;
                        v++;
                    }
                    if (v >= len)
                        break;
                    size_t vend = v;
                    size_t vallen = vend - vstart;

                    int idx = find_string_index_for_key(keybuf);
                    if (idx < 0)
                    {
                        idx = lookup_dotted_key(keybuf);
                    }
                    if (idx >= 0)
                    {
                        char *un = unescape_json_string(&s[vstart], vallen);
                        if (un)
                        {
                            s_dynamic_strings[idx] = un;
                        }
                    }

                    // advance i past this value
                    i = vend + 1;
                    continue;
                }
                else if (p < len && s[p] == '{')
                {
                    // value is an object; just advance inside it
                    i = p + 1;
                    continue;
                }
                else
                {
                    // not a string value, skip
                    i = p + 1;
                    continue;
                }
            }
            else
            {
                // quoted token not followed by colon - probably a value; skip
                i = kend + 1;
                continue;
            }
        }
        else
        {
            i++;
        }
    }

    if (unquoted)
    {
        free(unquoted);
    }
}

const char *localization_get_string(StringId string_id)
{
    if (string_id >= STRING_COUNT)
    {
        return "";
    }

    // Return dynamic string if provided from phone
    if (s_dynamic_strings[string_id])
    {
        return s_dynamic_strings[string_id];
    }

    const char *str = NULL;
    if (s_current_strings)
        str = s_current_strings[string_id];
    // No built-in English fallback available here; return empty string if missing
    return str ? str : "";
}

const char *localization_get_category_by_index(uint8_t index)
{
    // TODO: This index max should be based on the MAX_ITEMS for categories in the data file
    if (index >= 20)
    {
        return "";
    }
    return localization_get_string(STRING_CATEGORY_A + index);
}
