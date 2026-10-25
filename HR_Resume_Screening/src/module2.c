#include "module2.h"

#include "module1.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *const DEFAULT_SKILLS[] = {"c", "java", "python", "sql", "linux"};

static void clear_result(AnalysisResult *result)
{
    size_t index;

    result->skill_count = 0U;
    result->total_words = 0U;
    result->bytes_processed = 0U;
    for (index = 0U; index < MAX_SKILLS; ++index)
    {
        result->skills[index].name[0] = '\0';
        result->skills[index].frequency = 0U;
    }
}

static int process_skill_line(char *line, AnalysisResult *result)
{
    size_t start = 0U;
    size_t end = strlen(line);
    size_t index;
    int add_status;

    while ((start < end) && (isspace((unsigned char)line[start]) != 0))
    {
        ++start;
    }
    while ((end > start) && (isspace((unsigned char)line[end - 1U]) != 0))
    {
        --end;
    }
    if ((start == end) || (line[start] == '#'))
    {
        return 0;
    }
    line[end] = '\0';
    if (is_valid_skill_name(&line[start]) == 0)
    {
        return -1;
    }
    for (index = start; line[index] != '\0'; ++index)
    {
        line[index - start] = line[index];
    }
    line[index - start] = '\0';
    if (normalize_token(line, SKILL_NAME_CAPACITY, strlen(line)) != 0)
    {
        return -1;
    }
    add_status = add_skill(result, line);
    return (add_status < 0) ? -1 : 0;
}

void initialize_result(AnalysisResult *result)
{
    size_t index;

    if (result == NULL)
    {
        return;
    }
    clear_result(result);
    for (index = 0U; index < (sizeof(DEFAULT_SKILLS) / sizeof(DEFAULT_SKILLS[0])); ++index)
    {
        (void)add_skill(result, DEFAULT_SKILLS[index]);
    }
}

int add_skill(AnalysisResult *result, const char *skill_name)
{
    size_t index;
    char normalized_name[SKILL_NAME_CAPACITY];
    size_t name_length;

    if ((result == NULL) || (result->skill_count > MAX_SKILLS) ||
        (is_valid_skill_name(skill_name) == 0))
    {
        return -1;
    }
    name_length = strlen(skill_name);
    for (index = 0U; index < name_length; ++index)
    {
        normalized_name[index] = (char)tolower((unsigned char)skill_name[index]);
    }
    normalized_name[name_length] = '\0';
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (strcmp(result->skills[index].name, normalized_name) == 0)
        {
            return 1;
        }
    }
    if (result->skill_count >= MAX_SKILLS)
    {
        return -2;
    }
    (void)memcpy(result->skills[result->skill_count].name,
                 normalized_name, name_length + 1U);
    result->skills[result->skill_count].frequency = 0U;
    ++result->skill_count;
    return 0;
}

int load_skill_profile(const char *path, AnalysisResult *result)
{
    FILE *file;
    char line[MAX_SKILL_FILE_BYTES + 1U];
    size_t line_length = 0U;
    size_t bytes_read = 0U;
    int current;
    int status = ANALYSIS_OK;

    if ((path == NULL) || (path[0] == '\0') || (result == NULL))
    {
        return ANALYSIS_INVALID_INPUT;
    }
    file = fopen(path, "rb");
    if (file == NULL)
    {
        return ANALYSIS_IO_ERROR;
    }
    clear_result(result);
    while ((current = fgetc(file)) != EOF)
    {
        ++bytes_read;
        if (bytes_read > MAX_SKILL_FILE_BYTES)
        {
            status = ANALYSIS_PROFILE_TOO_LARGE;
            break;
        }
        if (current == '\n')
        {
            line[line_length] = '\0';
            if (process_skill_line(line, result) != 0)
            {
                status = ANALYSIS_INVALID_PROFILE;
                break;
            }
            line_length = 0U;
        }
        else if ((current != '\0') && (line_length < MAX_SKILL_FILE_BYTES))
        {
            line[line_length] = (char)current;
            ++line_length;
        }
        else
        {
            status = ANALYSIS_INVALID_PROFILE;
            break;
        }
    }
    if ((status == ANALYSIS_OK) && (line_length > 0U))
    {
        line[line_length] = '\0';
        if (process_skill_line(line, result) != 0)
        {
            status = ANALYSIS_INVALID_PROFILE;
        }
    }
    if ((ferror(file) != 0) && (status == ANALYSIS_OK))
    {
        status = ANALYSIS_IO_ERROR;
    }
    if (fclose(file) != 0)
    {
        status = ANALYSIS_IO_ERROR;
    }
    if ((status == ANALYSIS_OK) && (result->skill_count == 0U))
    {
        status = ANALYSIS_INVALID_PROFILE;
    }
    return status;
}

int analyze_resume(const char *path, AnalysisResult *result)
{
    if ((result == NULL) || (result->skill_count == 0U) ||
        (result->skill_count > MAX_SKILLS))
    {
        return ANALYSIS_INVALID_INPUT;
    }
    return read_resume(path, result);
}

size_t count_matched_skills(const AnalysisResult *result)
{
    size_t index;
    size_t matched = 0U;

    if ((result == NULL) || (result->skill_count > MAX_SKILLS))
    {
        return 0U;
    }
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (result->skills[index].frequency > 0U)
        {
            ++matched;
        }
    }
    return matched;
}

double calculate_score(const AnalysisResult *result)
{
    if ((result == NULL) || (result->skill_count == 0U) ||
        (result->skill_count > MAX_SKILLS))
    {
        return 0.0;
    }
    return ((double)count_matched_skills(result) * 100.0) /
           (double)result->skill_count;
}

const char *classify_score(double score)
{
    if (score >= 80.0)
    {
        return "Strong match";
    }
    if (score >= 50.0)
    {
        return "Potential match";
    }
    return "Low match";
}

int write_report(FILE *output, const AnalysisResult *result)
{
    size_t index;
    size_t sorted_indices[MAX_SKILLS];
    size_t matched_count;
    double score;

    if ((result == NULL) || (output == NULL) || (result->skill_count == 0U) ||
        (result->skill_count > MAX_SKILLS))
    {
        return -1;
    }
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (is_valid_skill_name(result->skills[index].name) == 0)
        {
            return -1;
        }
    }
    matched_count = count_matched_skills(result);
    score = calculate_score(result);
    for (index = 0U; index < result->skill_count; ++index)
    {
        size_t position = index;
        sorted_indices[index] = index;
        while ((position > 0U) &&
               (result->skills[sorted_indices[position - 1U]].frequency <
                result->skills[sorted_indices[position]].frequency))
        {
            size_t temporary = sorted_indices[position - 1U];
            sorted_indices[position - 1U] = sorted_indices[position];
            sorted_indices[position] = temporary;
            --position;
        }
    }

    (void)fprintf(output, "\n+--------------------------------------------------+\n");
    (void)fprintf(output, "|             HR RESUME SCREENING                 |\n");
    (void)fprintf(output, "+--------------------------------------------------+\n");
    (void)fprintf(output, "Required-skill coverage: %5.1f%%  (%zu of %zu)\n",
                  score, matched_count, result->skill_count);
    (void)fprintf(output, "Resume size: %zu bytes  |  Words analyzed: %zu\n",
                  result->bytes_processed, result->total_words);
    (void)fprintf(output, "\nMATCHED SKILLS\n");
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (result->skills[index].frequency > 0U)
        {
            (void)fprintf(output, "  [x] %-20s %zu mentions\n",
                          result->skills[index].name,
                          result->skills[index].frequency);
        }
    }
    if (matched_count == 0U)
    {
        (void)fprintf(output, "  None\n");
    }
    (void)fprintf(output, "\nSKILLS TO REVIEW\n");
    for (index = 0U; index < result->skill_count; ++index)
    {
        if (result->skills[index].frequency == 0U)
        {
            (void)fprintf(output, "  [ ] %s\n", result->skills[index].name);
        }
    }
    if (matched_count == result->skill_count)
    {
        (void)fprintf(output, "  None\n");
    }
    (void)fprintf(output, "\nKEYWORD FREQUENCY (highest first)\n");
    for (index = 0U; index < result->skill_count; ++index)
    {
        size_t skill_index = sorted_indices[index];
        (void)fprintf(output, "  %-20s %zu\n",
                      result->skills[skill_index].name,
                      result->skills[skill_index].frequency);
    }
    (void)fprintf(output, "\nSCREENING SUMMARY\n  %s\n", classify_score(score));
    (void)fprintf(output, "Scoring: each configured skill has equal weight.\n");
    (void)fprintf(output, "Decision support only; a human reviewer must verify evidence.\n");
    return (ferror(output) == 0) ? 0 : -1;
}
