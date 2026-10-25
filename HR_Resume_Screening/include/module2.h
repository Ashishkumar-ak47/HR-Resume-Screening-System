#ifndef MODULE2_H
#define MODULE2_H

#include "common.h"
#include <stdio.h>

void initialize_result(AnalysisResult *result);
int add_skill(AnalysisResult *result, const char *skill_name);
int load_skill_profile(const char *path, AnalysisResult *result);
int analyze_resume(const char *path, AnalysisResult *result);
size_t count_matched_skills(const AnalysisResult *result);
double calculate_score(const AnalysisResult *result);
const char *classify_score(double score);
int write_report(FILE *output, const AnalysisResult *result);

#endif
