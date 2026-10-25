#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stddef.h>

#define EMPLOYEE_ID_CAPACITY 32U
#define EMPLOYEE_NAME_CAPACITY 80U
#define EMPLOYEE_RESUME_PATH_CAPACITY 260U
#define MAX_EMPLOYEES 500U

typedef struct
{
    char id[EMPLOYEE_ID_CAPACITY];
    char name[EMPLOYEE_NAME_CAPACITY];
    char resume_path[EMPLOYEE_RESUME_PATH_CAPACITY];
} Employee;

typedef enum
{
    EMPLOYEE_OK = 0,
    EMPLOYEE_INVALID = -1,
    EMPLOYEE_NOT_FOUND = -2,
    EMPLOYEE_DUPLICATE = -3,
    EMPLOYEE_FULL = -4,
    EMPLOYEE_IO_ERROR = -5
} EmployeeStatus;

int employee_add(const char *database_path, const Employee *employee);
int employee_remove(const char *database_path, const char *employee_id);
int employee_set_resume(const char *database_path, const char *employee_id, const char *resume_path);
int employee_list(const char *database_path, Employee employees[], size_t capacity, size_t *count);
int employee_find(const char *database_path, const char *employee_id, Employee *employee);
int employee_is_valid(const Employee *employee);

#endif
