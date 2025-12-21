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
    // menus
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
    // categories
    "STRING_CATEGORY_THREE_D_PRINTING",
    "STRING_CATEGORY_AFRICA",
    "STRING_CATEGORY_AI",
    "STRING_CATEGORY_APPLE",
    "STRING_CATEGORY_ARGENTINA",
    "STRING_CATEGORY_AUSTRALIA",
    "STRING_CATEGORY_AUSTRIA",
    "STRING_CATEGORY_BAY",
    "STRING_CATEGORY_BELGIUM",
    "STRING_CATEGORY_BITCOIN",
    "STRING_CATEGORY_BRAZIL",
    "STRING_CATEGORY_BUSINESS",
    "STRING_CATEGORY_CANADA",
    "STRING_CATEGORY_CATHOLIC",
    "STRING_CATEGORY_CHINA",
    "STRING_CATEGORY_COFFEE",
    "STRING_CATEGORY_COLOMBIA",
    "STRING_CATEGORY_COSTA_RICA",
    "STRING_CATEGORY_CROATIA",
    "STRING_CATEGORY_CRYPTOCURRENCY",
    "STRING_CATEGORY_CYBERSECURITY",
    "STRING_CATEGORY_CZECH_REPUBLIC",
    "STRING_CATEGORY_DENMARK",
    "STRING_CATEGORY_ECONOMY",
    "STRING_CATEGORY_ESTONIA",
    "STRING_CATEGORY_EUROPE",
    "STRING_CATEGORY_FINLAND",
    "STRING_CATEGORY_FORMULA_1",
    "STRING_CATEGORY_FRANCE",
    "STRING_CATEGORY_FRENCH_MUSIC",
    "STRING_CATEGORY_GAMING",
    "STRING_CATEGORY_GERMANY_HESSE",
    "STRING_CATEGORY_GERMANY",
    "STRING_CATEGORY_GOOGLE",
    "STRING_CATEGORY_GREECE",
    "STRING_CATEGORY_HEALTHCARE_USA",
    "STRING_CATEGORY_HONG_KONG",
    "STRING_CATEGORY_HUNGARY",
    "STRING_CATEGORY_INDIA_TAMIL_NADU",
    "STRING_CATEGORY_INDIA",
    "STRING_CATEGORY_IRACING_AND_SIM_RACING",
    "STRING_CATEGORY_IRAN",
    "STRING_CATEGORY_IRELAND",
    "STRING_CATEGORY_ISRAEL",
    "STRING_CATEGORY_ITALY",
    "STRING_CATEGORY_JAPAN",
    "STRING_CATEGORY_LINUX_OSS",
    "STRING_CATEGORY_LITHUANIA",
    "STRING_CATEGORY_MEXICO",
    "STRING_CATEGORY_MICROSOFT",
    "STRING_CATEGORY_NEW_ZEALAND",
    "STRING_CATEGORY_NFL",
    "STRING_CATEGORY_NHL",
    "STRING_CATEGORY_NINTENDO",
    "STRING_CATEGORY_NORWAY",
    "STRING_CATEGORY_PAKISTAN",
    "STRING_CATEGORY_PALESTINE",
    "STRING_CATEGORY_PHILIPPINES",
    "STRING_CATEGORY_PODCASTING",
    "STRING_CATEGORY_POLAND",
    "STRING_CATEGORY_PORTUGAL",
    "STRING_CATEGORY_PRIVACY",
    "STRING_CATEGORY_ROMANIA",
    "STRING_CATEGORY_RUSSIA",
    "STRING_CATEGORY_SCIENCE",
    "STRING_CATEGORY_SERBIA",
    "STRING_CATEGORY_SIMULATION_GAMES",
    "STRING_CATEGORY_SINGAPORE",
    "STRING_CATEGORY_SLOVAKIA",
    "STRING_CATEGORY_SLOVENIA",
    "STRING_CATEGORY_SOUTH_KOREA",
    "STRING_CATEGORY_SPAIN",
    "STRING_CATEGORY_SPORTS",
    "STRING_CATEGORY_SWEDEN",
    "STRING_CATEGORY_SWITZERLAND_DE",
    "STRING_CATEGORY_SWITZERLAND_FR",
    "STRING_CATEGORY_TAIWAN",
    "STRING_CATEGORY_TECHNOLOGY",
    "STRING_CATEGORY_THAILAND",
    "STRING_CATEGORY_THE_NETHERLANDS",
    "STRING_CATEGORY_TURKEY",
    "STRING_CATEGORY_UK",
    "STRING_CATEGORY_UKRAINE",
    "STRING_CATEGORY_USA_AUSTIN",
    "STRING_CATEGORY_USA_CHICAGO",
    "STRING_CATEGORY_USA_COLORADO",
    "STRING_CATEGORY_USA_HAWAII",
    "STRING_CATEGORY_USA_MICHIGAN",
    "STRING_CATEGORY_USA_MINNESOTA",
    "STRING_CATEGORY_USA_NEVADA",
    "STRING_CATEGORY_USA_NYC",
    "STRING_CATEGORY_USA_OHIO",
    "STRING_CATEGORY_USA_UTAH",
    "STRING_CATEGORY_USA_VERMONT",
    "STRING_CATEGORY_USA_VIRGINIA",
    "STRING_CATEGORY_USA_WASHINGTON",
    "STRING_CATEGORY_USA",
    "STRING_CATEGORY_WATCHES_AND_HOROLOGY",
    "STRING_CATEGORY_WORLD",
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

    /* title */
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

    /* categories (keys match JS type names) */
    {"category.three_d_printing", STRING_CATEGORY_THREE_D_PRINTING},
    {"category.africa", STRING_CATEGORY_AFRICA},
    {"category.ai", STRING_CATEGORY_AI},
    {"category.apple", STRING_CATEGORY_APPLE},
    {"category.argentina", STRING_CATEGORY_ARGENTINA},
    {"category.australia", STRING_CATEGORY_AUSTRALIA},
    {"category.austria", STRING_CATEGORY_AUSTRIA},
    {"category.bay", STRING_CATEGORY_BAY},
    {"category.belgium", STRING_CATEGORY_BELGIUM},
    {"category.bitcoin", STRING_CATEGORY_BITCOIN},
    {"category.brazil", STRING_CATEGORY_BRAZIL},
    {"category.business", STRING_CATEGORY_BUSINESS},
    {"category.canada", STRING_CATEGORY_CANADA},
    {"category.catholic", STRING_CATEGORY_CATHOLIC},
    {"category.china", STRING_CATEGORY_CHINA},
    {"category.coffee", STRING_CATEGORY_COFFEE},
    {"category.colombia", STRING_CATEGORY_COLOMBIA},
    {"category.costa_rica", STRING_CATEGORY_COSTA_RICA},
    {"category.croatia", STRING_CATEGORY_CROATIA},
    {"category.cryptocurrency", STRING_CATEGORY_CRYPTOCURRENCY},
    {"category.cybersecurity", STRING_CATEGORY_CYBERSECURITY},
    {"category.czech_republic", STRING_CATEGORY_CZECH_REPUBLIC},
    {"category.denmark", STRING_CATEGORY_DENMARK},
    {"category.economy", STRING_CATEGORY_ECONOMY},
    {"category.estonia", STRING_CATEGORY_ESTONIA},
    {"category.europe", STRING_CATEGORY_EUROPE},
    {"category.finland", STRING_CATEGORY_FINLAND},
    {"category.formula_1", STRING_CATEGORY_FORMULA_1},
    {"category.france", STRING_CATEGORY_FRANCE},
    {"category.french_music", STRING_CATEGORY_FRENCH_MUSIC},
    {"category.gaming", STRING_CATEGORY_GAMING},
    {"category.germany_hesse", STRING_CATEGORY_GERMANY_HESSE},
    {"category.germany", STRING_CATEGORY_GERMANY},
    {"category.google", STRING_CATEGORY_GOOGLE},
    {"category.greece", STRING_CATEGORY_GREECE},
    {"category.healthcare_usa", STRING_CATEGORY_HEALTHCARE_USA},
    {"category.hong_kong", STRING_CATEGORY_HONG_KONG},
    {"category.hungary", STRING_CATEGORY_HUNGARY},
    {"category.india_tamil_nadu", STRING_CATEGORY_INDIA_TAMIL_NADU},
    {"category.india", STRING_CATEGORY_INDIA},
    {"category.iracing", STRING_CATEGORY_IRACING_AND_SIM_RACING},
    {"category.iran", STRING_CATEGORY_IRAN},
    {"category.ireland", STRING_CATEGORY_IRELAND},
    {"category.israel", STRING_CATEGORY_ISRAEL},
    {"category.italy", STRING_CATEGORY_ITALY},
    {"category.japan", STRING_CATEGORY_JAPAN},
    {"category.linux_oss", STRING_CATEGORY_LINUX_OSS},
    {"category.lithuania", STRING_CATEGORY_LITHUANIA},
    {"category.mexico", STRING_CATEGORY_MEXICO},
    {"category.microsoft", STRING_CATEGORY_MICROSOFT},
    {"category.new_zealand", STRING_CATEGORY_NEW_ZEALAND},
    {"category.nfl", STRING_CATEGORY_NFL},
    {"category.nhl", STRING_CATEGORY_NHL},
    {"category.nintendo", STRING_CATEGORY_NINTENDO},
    {"category.norway", STRING_CATEGORY_NORWAY},
    {"category.pakistan", STRING_CATEGORY_PAKISTAN},
    {"category.palestine", STRING_CATEGORY_PALESTINE},
    {"category.philippines", STRING_CATEGORY_PHILIPPINES},
    {"category.podcasting", STRING_CATEGORY_PODCASTING},
    {"category.poland", STRING_CATEGORY_POLAND},
    {"category.portugal", STRING_CATEGORY_PORTUGAL},
    {"category.privacy", STRING_CATEGORY_PRIVACY},
    {"category.romania", STRING_CATEGORY_ROMANIA},
    {"category.russia", STRING_CATEGORY_RUSSIA},
    {"category.science", STRING_CATEGORY_SCIENCE},
    {"category.serbia", STRING_CATEGORY_SERBIA},
    {"category.simulation_games", STRING_CATEGORY_SIMULATION_GAMES},
    {"category.singapore", STRING_CATEGORY_SINGAPORE},
    {"category.slovakia", STRING_CATEGORY_SLOVAKIA},
    {"category.slovenia", STRING_CATEGORY_SLOVENIA},
    {"category.south_korea", STRING_CATEGORY_SOUTH_KOREA},
    {"category.spain", STRING_CATEGORY_SPAIN},
    {"category.sports", STRING_CATEGORY_SPORTS},
    {"category.sweden", STRING_CATEGORY_SWEDEN},
    {"category.switzerland_de", STRING_CATEGORY_SWITZERLAND_DE},
    {"category.switzerland_fr", STRING_CATEGORY_SWITZERLAND_FR},
    {"category.taiwan", STRING_CATEGORY_TAIWAN},
    {"category.technology", STRING_CATEGORY_TECHNOLOGY},
    {"category.thailand", STRING_CATEGORY_THAILAND},
    {"category.the_netherlands", STRING_CATEGORY_THE_NETHERLANDS},
    {"category.turkey", STRING_CATEGORY_TURKEY},
    {"category.uk", STRING_CATEGORY_UK},
    {"category.ukraine", STRING_CATEGORY_UKRAINE},
    {"category.usa_austin", STRING_CATEGORY_USA_AUSTIN},
    {"category.usa_chicago", STRING_CATEGORY_USA_CHICAGO},
    {"category.usa_colorado", STRING_CATEGORY_USA_COLORADO},
    {"category.usa_hawaii", STRING_CATEGORY_USA_HAWAII},
    {"category.usa_michigan", STRING_CATEGORY_USA_MICHIGAN},
    {"category.usa_minnesota", STRING_CATEGORY_USA_MINNESOTA},
    {"category.usa_nevada", STRING_CATEGORY_USA_NEVADA},
    {"category.usa_nyc", STRING_CATEGORY_USA_NYC},
    {"category.usa_ohio", STRING_CATEGORY_USA_OHIO},
    {"category.usa_utah", STRING_CATEGORY_USA_UTAH},
    {"category.usa_vermont", STRING_CATEGORY_USA_VERMONT},
    {"category.usa_virginia", STRING_CATEGORY_USA_VIRGINIA},
    {"category.usa_washington", STRING_CATEGORY_USA_WASHINGTON},
    {"category.usa", STRING_CATEGORY_USA},
    {"category.watches_and_horology", STRING_CATEGORY_WATCHES_AND_HOROLOGY},
    {"category.world", STRING_CATEGORY_WORLD},
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

const char *localization_translate_category(const char *category_name)
{
    if (!category_name || strlen(category_name) == 0)
    {
        return category_name;
    }

    // Map category names to StringId enums
    StringId string_id = STRING_COUNT;

    if (strcmp(category_name, "AI") == 0)
        string_id = STRING_CATEGORY_AI;
    else if (strcmp(category_name, "Africa") == 0)
        string_id = STRING_CATEGORY_AFRICA;
    else if (strcmp(category_name, "Apple") == 0)
        string_id = STRING_CATEGORY_APPLE;
    else if (strcmp(category_name, "Australia") == 0)
        string_id = STRING_CATEGORY_AUSTRALIA;
    else if (strcmp(category_name, "Austria") == 0)
        string_id = STRING_CATEGORY_AUSTRIA;
    else if (strcmp(category_name, "Bay") == 0)
        string_id = STRING_CATEGORY_BAY;
    else if (strcmp(category_name, "Belgium") == 0)
        string_id = STRING_CATEGORY_BELGIUM;
    else if (strcmp(category_name, "Bitcoin") == 0)
        string_id = STRING_CATEGORY_BITCOIN;
    else if (strcmp(category_name, "Brazil") == 0)
        string_id = STRING_CATEGORY_BRAZIL;
    else if (strcmp(category_name, "Business") == 0)
        string_id = STRING_CATEGORY_BUSINESS;
    else if (strcmp(category_name, "Canada") == 0)
        string_id = STRING_CATEGORY_CANADA;
    else if (strcmp(category_name, "Catholic") == 0)
        string_id = STRING_CATEGORY_CATHOLIC;
    else if (strcmp(category_name, "China") == 0)
        string_id = STRING_CATEGORY_CHINA;
    else if (strcmp(category_name, "Coffee") == 0)
        string_id = STRING_CATEGORY_COFFEE;
    else if (strcmp(category_name, "Colombia") == 0)
        string_id = STRING_CATEGORY_COLOMBIA;
    else if (strcmp(category_name, "Costa Rica") == 0)
        string_id = STRING_CATEGORY_COSTA_RICA;
    else if (strcmp(category_name, "Croatia") == 0)
        string_id = STRING_CATEGORY_CROATIA;
    else if (strcmp(category_name, "Cryptocurrency") == 0)
        string_id = STRING_CATEGORY_CRYPTOCURRENCY;
    else if (strcmp(category_name, "Cybersecurity") == 0)
        string_id = STRING_CATEGORY_CYBERSECURITY;
    else if (strcmp(category_name, "Czech Republic") == 0)
        string_id = STRING_CATEGORY_CZECH_REPUBLIC;
    else if (strcmp(category_name, "Denmark") == 0)
        string_id = STRING_CATEGORY_DENMARK;
    else if (strcmp(category_name, "Economy") == 0)
        string_id = STRING_CATEGORY_ECONOMY;
    else if (strcmp(category_name, "Estonia") == 0)
        string_id = STRING_CATEGORY_ESTONIA;
    else if (strcmp(category_name, "Europe") == 0)
        string_id = STRING_CATEGORY_EUROPE;
    else if (strcmp(category_name, "Finland") == 0)
        string_id = STRING_CATEGORY_FINLAND;
    else if (strcmp(category_name, "Formula 1") == 0)
        string_id = STRING_CATEGORY_FORMULA_1;
    else if (strcmp(category_name, "France") == 0)
        string_id = STRING_CATEGORY_FRANCE;
    else if (strcmp(category_name, "Gaming") == 0)
        string_id = STRING_CATEGORY_GAMING;
    else if (strcmp(category_name, "Germany | Hesse") == 0)
        string_id = STRING_CATEGORY_GERMANY_HESSE;
    else if (strcmp(category_name, "Germany") == 0)
        string_id = STRING_CATEGORY_GERMANY;
    else if (strcmp(category_name, "Greece") == 0)
        string_id = STRING_CATEGORY_GREECE;
    else if (strcmp(category_name, "Hungary") == 0)
        string_id = STRING_CATEGORY_HUNGARY;
    else if (strcmp(category_name, "India") == 0)
        string_id = STRING_CATEGORY_INDIA;
    else if (strcmp(category_name, "Iran") == 0)
        string_id = STRING_CATEGORY_IRAN;
    else if (strcmp(category_name, "Ireland") == 0)
        string_id = STRING_CATEGORY_IRELAND;
    else if (strcmp(category_name, "Israel") == 0)
        string_id = STRING_CATEGORY_ISRAEL;
    else if (strcmp(category_name, "Italy") == 0)
        string_id = STRING_CATEGORY_ITALY;
    else if (strcmp(category_name, "Japan") == 0)
        string_id = STRING_CATEGORY_JAPAN;
    else if (strcmp(category_name, "Linux & OSS") == 0)
        string_id = STRING_CATEGORY_LINUX_OSS;
    else if (strcmp(category_name, "Lithuania") == 0)
        string_id = STRING_CATEGORY_LITHUANIA;
    else if (strcmp(category_name, "Mexico") == 0)
        string_id = STRING_CATEGORY_MEXICO;
    else if (strcmp(category_name, "NFL") == 0)
        string_id = STRING_CATEGORY_NFL;
    else if (strcmp(category_name, "NHL") == 0)
        string_id = STRING_CATEGORY_NHL;
    else if (strcmp(category_name, "New Zealand") == 0)
        string_id = STRING_CATEGORY_NEW_ZEALAND;
    else if (strcmp(category_name, "Nintendo") == 0)
        string_id = STRING_CATEGORY_NINTENDO;
    else if (strcmp(category_name, "Norway") == 0)
        string_id = STRING_CATEGORY_NORWAY;
    else if (strcmp(category_name, "Pakistan") == 0)
        string_id = STRING_CATEGORY_PAKISTAN;
    else if (strcmp(category_name, "Palestine") == 0)
        string_id = STRING_CATEGORY_PALESTINE;
    else if (strcmp(category_name, "Philippines") == 0)
        string_id = STRING_CATEGORY_PHILIPPINES;
    else if (strcmp(category_name, "Podcasting") == 0)
        string_id = STRING_CATEGORY_PODCASTING;
    else if (strcmp(category_name, "Poland") == 0)
        string_id = STRING_CATEGORY_POLAND;
    else if (strcmp(category_name, "Portugal") == 0)
        string_id = STRING_CATEGORY_PORTUGAL;
    else if (strcmp(category_name, "Romania") == 0)
        string_id = STRING_CATEGORY_ROMANIA;
    else if (strcmp(category_name, "Russia") == 0)
        string_id = STRING_CATEGORY_RUSSIA;
    else if (strcmp(category_name, "Science") == 0)
        string_id = STRING_CATEGORY_SCIENCE;
    else if (strcmp(category_name, "Serbia") == 0)
        string_id = STRING_CATEGORY_SERBIA;
    else if (strcmp(category_name, "Simulation Games") == 0)
        string_id = STRING_CATEGORY_SIMULATION_GAMES;
    else if (strcmp(category_name, "Singapore") == 0)
        string_id = STRING_CATEGORY_SINGAPORE;
    else if (strcmp(category_name, "Slovakia") == 0)
        string_id = STRING_CATEGORY_SLOVAKIA;
    else if (strcmp(category_name, "Slovenia") == 0)
        string_id = STRING_CATEGORY_SLOVENIA;
    else if (strcmp(category_name, "South Korea") == 0)
        string_id = STRING_CATEGORY_SOUTH_KOREA;
    else if (strcmp(category_name, "Spain") == 0)
        string_id = STRING_CATEGORY_SPAIN;
    else if (strcmp(category_name, "Sports") == 0)
        string_id = STRING_CATEGORY_SPORTS;
    else if (strcmp(category_name, "Sweden") == 0)
        string_id = STRING_CATEGORY_SWEDEN;
    else if (strcmp(category_name, "Switzerland (DE)") == 0)
        string_id = STRING_CATEGORY_SWITZERLAND_DE;
    else if (strcmp(category_name, "Switzerland (FR)") == 0)
        string_id = STRING_CATEGORY_SWITZERLAND_FR;
    else if (strcmp(category_name, "Taiwan") == 0)
        string_id = STRING_CATEGORY_TAIWAN;
    else if (strcmp(category_name, "Technology") == 0)
        string_id = STRING_CATEGORY_TECHNOLOGY;
    else if (strcmp(category_name, "Thailand") == 0)
        string_id = STRING_CATEGORY_THAILAND;
    else if (strcmp(category_name, "The Netherlands") == 0)
        string_id = STRING_CATEGORY_THE_NETHERLANDS;
    else if (strcmp(category_name, "Turkey") == 0)
        string_id = STRING_CATEGORY_TURKEY;
    else if (strcmp(category_name, "UK") == 0)
        string_id = STRING_CATEGORY_UK;
    else if (strcmp(category_name, "Ukraine") == 0)
        string_id = STRING_CATEGORY_UKRAINE;
    else if (strcmp(category_name, "USA | Austin, TX") == 0)
        string_id = STRING_CATEGORY_USA_AUSTIN;
    else if (strcmp(category_name, "USA | Chicago") == 0)
        string_id = STRING_CATEGORY_USA_CHICAGO;
    else if (strcmp(category_name, "USA | Colorado") == 0)
        string_id = STRING_CATEGORY_USA_COLORADO;
    else if (strcmp(category_name, "USA | Michigan") == 0)
        string_id = STRING_CATEGORY_USA_MICHIGAN;
    else if (strcmp(category_name, "USA | Minnesota") == 0)
        string_id = STRING_CATEGORY_USA_MINNESOTA;
    else if (strcmp(category_name, "USA | Nevada") == 0)
        string_id = STRING_CATEGORY_USA_NEVADA;
    else if (strcmp(category_name, "USA | New York City") == 0)
        string_id = STRING_CATEGORY_USA_NYC;
    else if (strcmp(category_name, "USA | Ohio") == 0)
        string_id = STRING_CATEGORY_USA_OHIO;
    else if (strcmp(category_name, "USA | Utah") == 0)
        string_id = STRING_CATEGORY_USA_UTAH;
    else if (strcmp(category_name, "USA | Virginia") == 0)
        string_id = STRING_CATEGORY_USA_VIRGINIA;
    else if (strcmp(category_name, "USA") == 0)
        string_id = STRING_CATEGORY_USA;
    else if (strcmp(category_name, "World") == 0)
        string_id = STRING_CATEGORY_WORLD;
    else if (strcmp(category_name, "iRacing & Sim Racing") == 0)
        string_id = STRING_CATEGORY_IRACING_AND_SIM_RACING;
    else if (strcmp(category_name, "3D Printing") == 0)
        string_id = STRING_CATEGORY_THREE_D_PRINTING;
    else if (strcmp(category_name, "Argentina") == 0)
        string_id = STRING_CATEGORY_ARGENTINA;
    else if (strcmp(category_name, "Google") == 0)
        string_id = STRING_CATEGORY_GOOGLE;
    else if (strcmp(category_name, "Healthcare | USA") == 0)
        string_id = STRING_CATEGORY_HEALTHCARE_USA;
    else if (strcmp(category_name, "Hong Kong") == 0)
        string_id = STRING_CATEGORY_HONG_KONG;
    else if (strcmp(category_name, "India | Tamil Nadu") == 0)
        string_id = STRING_CATEGORY_INDIA_TAMIL_NADU;
    else if (strcmp(category_name, "Microsoft") == 0)
        string_id = STRING_CATEGORY_MICROSOFT;
    else if (strcmp(category_name, "Watches & Horology") == 0)
        string_id = STRING_CATEGORY_WATCHES_AND_HOROLOGY;
    else if (strcmp(category_name, "USA | Hawaii") == 0)
        string_id = STRING_CATEGORY_USA_HAWAII;
    else if (strcmp(category_name, "USA | Vermont") == 0)
        string_id = STRING_CATEGORY_USA_VERMONT;
    else if (strcmp(category_name, "USA | Washington") == 0)
        string_id = STRING_CATEGORY_USA_WASHINGTON;
    else if (strcmp(category_name, "Privacy") == 0)
        string_id = STRING_CATEGORY_PRIVACY;
    // If no mapping found, return original name
    if (string_id == STRING_COUNT)
    {
        return category_name;
    }

    // Return translated string
    return localization_get_string(string_id);
}
