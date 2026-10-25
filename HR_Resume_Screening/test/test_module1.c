#include "module1.h"
#include "module2.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void write_repeated_bytes(const char *path, size_t length)
{
    FILE *file = fopen(path, "wb");
    char block[4096];
    size_t remaining = length;

    assert(file != NULL);
    (void)memset(block, 'x', sizeof(block));
    while (remaining > 0U)
    {
        size_t amount = (remaining < sizeof(block)) ? remaining : sizeof(block);
        assert(fwrite(block, 1U, amount, file) == amount);
        remaining -= amount;
    }
    assert(fclose(file) == 0);
}

int main(void)
{
    const char *path = "test_resume_module1.txt";
    AnalysisResult result;
    char token[8] = "PYTHON";
    FILE *file;

    assert(is_word_character('A') != 0);
    assert(is_word_character('7') != 0);
    assert(is_word_character('!') == 0);
    assert(normalize_token(token, sizeof(token), 6U) == 0);
    assert(strcmp(token, "python") == 0);
    assert(normalize_token(NULL, sizeof(token), 0U) == -1);
    assert(normalize_token(token, 6U, 6U) == -1);
    assert(normalize_token(token, 0U, 0U) == -1);

    initialize_result(&result);
    file = fopen(path, "w");
    assert(file != NULL);
    (void)fputs("PYTHON python! C, c. Java SQL Linux Pythonista", file);
    assert(fclose(file) == 0);
    assert(read_resume(path, &result) == ANALYSIS_OK);
    assert(result.total_words == 8U);
    assert(result.bytes_processed == strlen("PYTHON python! C, c. Java SQL Linux Pythonista"));
    assert(result.skills[0].frequency == 2U);
    assert(result.skills[1].frequency == 1U);
    assert(result.skills[2].frequency == 2U);
    assert(result.skills[3].frequency == 1U);
    assert(result.skills[4].frequency == 1U);
    assert(analyze_resume(path, &result) == ANALYSIS_OK);
    assert(result.skills[2].frequency == 2U);
    (void)remove(path);

    assert(read_resume(NULL, &result) == ANALYSIS_INVALID_INPUT);
    assert(read_resume("", &result) == ANALYSIS_INVALID_INPUT);
    assert(read_resume(path, NULL) == ANALYSIS_INVALID_INPUT);
    assert(read_resume("missing_resume_module1.txt", &result) == ANALYSIS_IO_ERROR);
    result.skill_count = 0U;
    assert(read_resume(path, &result) == ANALYSIS_INVALID_INPUT);
    initialize_result(&result);

    file = fopen(path, "wb");
    assert(file != NULL);
    assert(fclose(file) == 0);
    assert(read_resume(path, &result) == ANALYSIS_EMPTY);
    (void)remove(path);

    write_repeated_bytes(path, 1024U * 1024U);
    assert(read_resume(path, &result) == ANALYSIS_OK);
    assert(result.bytes_processed == (1024U * 1024U));
    (void)remove(path);

    write_repeated_bytes(path, MAX_RESUME_BYTES);
    assert(read_resume(path, &result) == ANALYSIS_OK);
    assert(result.bytes_processed == MAX_RESUME_BYTES);
    (void)remove(path);
    write_repeated_bytes(path, MAX_RESUME_BYTES + 1U);
    assert(read_resume(path, &result) == ANALYSIS_RESUME_TOO_LARGE);
    (void)remove(path);

    (void)puts("test_module1: all checks passed");
    return 0;
}
