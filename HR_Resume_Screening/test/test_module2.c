#include "module2.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void write_text_file(const char *path, const char *contents)
{
    FILE *file = fopen(path, "wb");
    assert(file != NULL);
    assert(fputs(contents, file) >= 0);
    assert(fclose(file) == 0);
}

static void test_skill_profiles(void)
{
    const char *path = "test_skills_module2.txt";
    AnalysisResult result;
    FILE *file;

    initialize_result(&result);
    assert(is_valid_skill_name("C") == 1);
    assert(is_valid_skill_name(NULL) == 0);
    assert(is_valid_skill_name("two words") == 0);
    assert(is_valid_skill_name("C++") == 0);
    assert(result.skill_count == 5U);
    assert(strcmp(result.skills[0].name, "c") == 0);
    assert(result.total_words == 0U);
    assert(result.bytes_processed == 0U);
    initialize_result(NULL);

    assert(add_skill(&result, "Go") == 0);
    assert(strcmp(result.skills[5].name, "go") == 0);
    assert(add_skill(&result, "GO") == 1);
    assert(add_skill(&result, NULL) == -1);
    assert(add_skill(&result, "C++") == -1);
    assert(add_skill(&result, "two words") == -1);

    write_text_file(path, "# role skills\n\n C \nPython\nPYTHON\nSQL\n");
    assert(load_skill_profile(path, &result) == ANALYSIS_OK);
    assert(result.skill_count == 3U);
    assert(strcmp(result.skills[0].name, "c") == 0);
    assert(strcmp(result.skills[1].name, "python") == 0);
    assert(strcmp(result.skills[2].name, "sql") == 0);
    (void)remove(path);

    write_text_file(path, "");
    assert(load_skill_profile(path, &result) == ANALYSIS_INVALID_PROFILE);
    (void)remove(path);
    write_text_file(path, "C++\n");
    assert(load_skill_profile(path, &result) == ANALYSIS_INVALID_PROFILE);
    (void)remove(path);
    assert(load_skill_profile("missing_skills_module2.txt", &result) == ANALYSIS_IO_ERROR);
    assert(load_skill_profile(NULL, &result) == ANALYSIS_INVALID_INPUT);
    assert(load_skill_profile(path, NULL) == ANALYSIS_INVALID_INPUT);

    file = fopen(path, "wb");
    assert(file != NULL);
    for (size_t index = 0U; index < ((MAX_SKILL_FILE_BYTES / 2U) + 1U); ++index)
    {
        assert(fputs("x\n", file) >= 0);
    }
    assert(fclose(file) == 0);
    assert(load_skill_profile(path, &result) == ANALYSIS_PROFILE_TOO_LARGE);
    (void)remove(path);

    initialize_result(&result);
    result.skill_count = 0U;
    for (size_t index = 0U; index < MAX_SKILLS; ++index)
    {
        char name[SKILL_NAME_CAPACITY];
        (void)snprintf(name, sizeof(name), "skill%02u", (unsigned int)index);
        assert(add_skill(&result, name) == 0);
    }
    assert(add_skill(&result, "another") == -2);
    result.skill_count = MAX_SKILLS + 1U;
    assert(add_skill(&result, "invalidstate") == -1);
}

static void test_analysis_and_scoring(void)
{
    const char *resume_path = "test_resume_module2.txt";
    const char *profile_path = "test_single_skill_module2.txt";
    const char *report_path = "test_report_module2.txt";
    AnalysisResult result;
    FILE *report;
    char report_text[4096];
    size_t bytes_read;

    initialize_result(&result);
    assert(analyze_resume(NULL, &result) == ANALYSIS_INVALID_INPUT);
    assert(analyze_resume(resume_path, NULL) == ANALYSIS_INVALID_INPUT);
    result.skill_count = 0U;
    assert(analyze_resume(resume_path, &result) == ANALYSIS_INVALID_INPUT);
    assert(count_matched_skills(NULL) == 0U);
    assert(calculate_score(NULL) == 0.0);
    assert(classify_score(0.0)[0] == 'L');

    initialize_result(&result);
    assert(count_matched_skills(&result) == 0U);
    assert(calculate_score(&result) == 0.0);
    assert(strcmp(classify_score(49.9), "Low match") == 0);
    assert(strcmp(classify_score(50.0), "Potential match") == 0);
    assert(strcmp(classify_score(79.9), "Potential match") == 0);
    assert(strcmp(classify_score(80.0), "Strong match") == 0);

    for (size_t index = 0U; index < result.skill_count; ++index)
    {
        result.skills[index].frequency = 1U;
    }
    assert(count_matched_skills(&result) == 5U);
    assert(calculate_score(&result) == 100.0);
    assert(strcmp(classify_score(calculate_score(&result)), "Strong match") == 0);

    write_text_file(profile_path, "Python\n");
    assert(load_skill_profile(profile_path, &result) == ANALYSIS_OK);
    write_text_file(resume_path, "Pythonista python");
    assert(analyze_resume(resume_path, &result) == ANALYSIS_OK);
    assert(result.skill_count == 1U);
    assert(result.skills[0].frequency == 1U);
    assert(calculate_score(&result) == 100.0);
    (void)remove(profile_path);

    initialize_result(&result);
    write_text_file(resume_path, "random text with no required skills");
    assert(analyze_resume(resume_path, &result) == ANALYSIS_OK);
    assert(count_matched_skills(&result) == 0U);
    assert(calculate_score(&result) == 0.0);
    assert(strcmp(classify_score(calculate_score(&result)), "Low match") == 0);

    write_text_file(resume_path, "C Python SQL");
    assert(analyze_resume(resume_path, &result) == ANALYSIS_OK);
    assert(count_matched_skills(&result) == 3U);
    assert(calculate_score(&result) == 60.0);
    assert(strcmp(classify_score(calculate_score(&result)), "Potential match") == 0);
    report = fopen(report_path, "wb");
    assert(report != NULL);
    assert(write_report(report, &result) == 0);
    assert(fclose(report) == 0);
    assert(write_report(NULL, &result) == -1);
    assert(write_report(stdout, NULL) == -1);

    report = fopen(report_path, "rb");
    assert(report != NULL);
    bytes_read = fread(report_text, 1U, sizeof(report_text) - 1U, report);
    report_text[bytes_read] = '\0';
    assert(fclose(report) == 0);
    assert(strstr(report_text, "60.0%") != NULL);
    assert(strstr(report_text, "python") != NULL);
    assert(strstr(report_text, "Potential match") != NULL);
    (void)remove(report_path);
    (void)remove(resume_path);
}

int main(void)
{
    test_skill_profiles();
    test_analysis_and_scoring();
    (void)puts("test_module2: all checks passed");
    return 0;
}
