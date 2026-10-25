#include "module1.h"

#include "utils.h"

#include <stdio.h>
#include <string.h>

static void count_token(const char *token, AnalysisResult *result)
{
    size_t index;

    ++result->total_words;
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (strcmp(result->skills[index].name, token) == 0)
        {
            ++result->skills[index].frequency;
        }
    }
}

static void finish_token(char *token, size_t *token_length, AnalysisResult *result)
{
    if (*token_length > 0U)
    {
        if (normalize_token(token, TOKEN_CAPACITY, *token_length) == 0)
        {
            count_token(token, result);
        }
        *token_length = 0U;
    }
}

static int has_valid_skills(const AnalysisResult *result)
{
    size_t outer;

    if ((result == NULL) || (result->skill_count == 0U) ||
        (result->skill_count > MAX_SKILLS))
    {
        return 0;
    }
    for (outer = 0U; outer < result->skill_count; ++outer)
    {
        size_t inner;
        if (is_valid_skill_name(result->skills[outer].name) == 0)
        {
            return 0;
        }
        for (inner = outer + 1U; inner < result->skill_count; ++inner)
        {
            if (strcmp(result->skills[outer].name, result->skills[inner].name) == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

int read_resume(const char *path, AnalysisResult *result)
{
    FILE *file;
    size_t bytes_read = 0U;
    size_t token_length = 0U;
    char token[TOKEN_CAPACITY];
    int current;
    int status = ANALYSIS_OK;

    if ((path == NULL) || (path[0] == '\0') || (has_valid_skills(result) == 0))
    {
        return ANALYSIS_INVALID_INPUT;
    }
    file = fopen(path, "rb");
    if (file == NULL)
    {
        return ANALYSIS_IO_ERROR;
    }

    result->total_words = 0U;
    result->bytes_processed = 0U;
    for (size_t index = 0U; index < result->skill_count; ++index)
    {
        result->skills[index].frequency = 0U;
    }

    while ((current = fgetc(file)) != EOF)
    {
        ++bytes_read;
        if (bytes_read > MAX_RESUME_BYTES)
        {
            status = ANALYSIS_RESUME_TOO_LARGE;
            break;
        }
        if (is_word_character(current) != 0)
        {
            if (token_length < (TOKEN_CAPACITY - 1U))
            {
                token[token_length] = (char)current;
                ++token_length;
            }
        }
        else
        {
            finish_token(token, &token_length, result);
        }
    }
    if ((status == ANALYSIS_OK) && (token_length > 0U))
    {
        finish_token(token, &token_length, result);
    }
    if ((ferror(file) != 0) && (status == ANALYSIS_OK))
    {
        status = ANALYSIS_IO_ERROR;
    }
    if (fclose(file) != 0)
    {
        status = ANALYSIS_IO_ERROR;
    }
    result->bytes_processed = bytes_read;
    if ((status == ANALYSIS_OK) && (bytes_read == 0U))
    {
        return ANALYSIS_EMPTY;
    }
    return status;
}
