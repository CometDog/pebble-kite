#include "string_utils.h"
#include <string.h>

char *string_duplicate(const char *src)
{
    if (!src)
        return NULL;

    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL)
        return NULL;

    strcpy(dest, src);
    return dest;
}

char *string_trim(char *str)
{
    if (!str)
        return str;

    // Trim leading spaces
    while (*str == ' ')
        str++;

    if (*str == 0)
        return str;

    // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while (end > str && *end == ' ')
        end--;
    *(end + 1) = '\0';

    return str;
}

int8_t string_split_to_array(char **array, int8_t current_count, int8_t max_capacity, char *string,
                             const char *delimiter)
{
    if (!array || !string || !delimiter || current_count >= max_capacity)
        return 0;

    char *tmp_string = string_duplicate(string);
    if (!tmp_string)
        return 0;

    int8_t item_count = 0;
    char *current = tmp_string;
    char *delim_pos;
    size_t delim_len = strlen(delimiter);

    while ((delim_pos = strstr(current, delimiter)) != NULL && current_count + item_count < max_capacity)
    {
        *delim_pos = '\0';

        char *trimmed = string_trim(current);
        if (strlen(trimmed) > 0)
        {
            array[current_count + item_count] = string_duplicate(trimmed);
            if (array[current_count + item_count])
                item_count++;
        }

        current = delim_pos + delim_len;
    }

    // Handle the final segment
    if (strlen(current) > 0 && current_count + item_count < max_capacity)
    {
        char *trimmed = string_trim(current);
        if (strlen(trimmed) > 0)
        {
            array[current_count + item_count] = string_duplicate(trimmed);
            if (array[current_count + item_count])
                item_count++;
        }
    }

    free(tmp_string);
    return item_count;
}