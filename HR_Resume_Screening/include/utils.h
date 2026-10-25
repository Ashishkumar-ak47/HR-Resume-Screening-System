#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

int is_word_character(int character);
int normalize_token(char *token, size_t capacity, size_t length);
int is_valid_skill_name(const char *skill_name);

#endif
