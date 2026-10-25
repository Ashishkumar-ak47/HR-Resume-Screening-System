#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#define MAX_SKILLS 32U
#define SKILL_NAME_CAPACITY 32U
#define TOKEN_CAPACITY 64U
#define MAX_RESUME_BYTES (5U * 1024U * 1024U)
#define MAX_SKILL_FILE_BYTES (16U * 1024U)

typedef enum
{
    ANALYSIS_OK = 0,
    ANALYSIS_EMPTY = 1,
    ANALYSIS_INVALID_INPUT = -1,
    ANALYSIS_RESUME_TOO_LARGE = -2,
    ANALYSIS_IO_ERROR = -3,
    ANALYSIS_INVALID_PROFILE = -4,
    ANALYSIS_PROFILE_TOO_LARGE = -5
} AnalysisStatus;

typedef struct
{
    char name[SKILL_NAME_CAPACITY];
    size_t frequency;
} SkillCount;

typedef struct
{
    SkillCount skills[MAX_SKILLS];
    size_t skill_count;
    size_t total_words;
    size_t bytes_processed;
} AnalysisResult;

#endif
