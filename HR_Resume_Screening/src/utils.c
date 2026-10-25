#include "utils.h"

#include "common.h"

#include <ctype.h>

int is_word_character(int character)
{
    return (isalnum((unsigned char)character) != 0);
}

int normalize_token(char *token, size_t capacity, size_t length)
{
    size_t index;

    if ((token == NULL) || (capacity == 0U) || (length >= capacity))
    {
        return -1;
    }
    for (index = 0U; index < length; ++index)
    {
        token[index] = (char)tolower((unsigned char)token[index]);
    }
    token[length] = '\0';
    return 0;
}

int is_valid_skill_name(const char *skill_name)
{
    size_t index = 0U;

    if ((skill_name == NULL) || (skill_name[0] == '\0'))
    {
        return 0;
    }
    while (skill_name[index] != '\0')
    {
        if ((index >= (SKILL_NAME_CAPACITY - 1U)) ||
            (is_word_character((unsigned char)skill_name[index]) == 0))
        {
            return 0;
        }
        ++index;
    }
    return 1;
}
