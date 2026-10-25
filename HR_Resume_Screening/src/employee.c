#include "employee.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define EMPLOYEE_LINE_CAPACITY (EMPLOYEE_ID_CAPACITY + EMPLOYEE_NAME_CAPACITY + EMPLOYEE_RESUME_PATH_CAPACITY + 3U)

static int safe_field(const char *value, size_t capacity, int allow_empty)
{
    size_t index;

    if ((value == NULL) || ((allow_empty == 0) && (value[0] == '\0')) || (strlen(value) >= capacity))
    {
        return 0;
    }
    for (index = 0U; value[index] != '\0'; ++index)
    {
        if ((value[index] == '|') || (value[index] == '\n') || (value[index] == '\r'))
        {
            return 0;
        }
    }
    return 1;
}

int employee_is_valid(const Employee *employee)
{
    size_t index;

    if ((employee == NULL) ||
        (safe_field(employee->id, EMPLOYEE_ID_CAPACITY, 0) == 0) ||
        (safe_field(employee->name, EMPLOYEE_NAME_CAPACITY, 0) == 0) ||
        (safe_field(employee->resume_path, EMPLOYEE_RESUME_PATH_CAPACITY, 1) == 0))
    {
        return 0;
    }
    for (index = 0U; employee->id[index] != '\0'; ++index)
    {
        const char character = employee->id[index];
        if (!(((character >= 'A') && (character <= 'Z')) ||
              ((character >= 'a') && (character <= 'z')) ||
              ((character >= '0') && (character <= '9')) || (character == '-') || (character == '_')))
        {
            return 0;
        }
    }
    return 1;
}

static int parse_line(char *line, Employee *employee)
{
    char *first;
    char *second;
    const size_t length = strlen(line);

    if ((length == 0U) || (line[length - 1U] != '\n'))
    {
        return 0;
    }
    line[length - 1U] = '\0';
    first = strchr(line, '|');
    if (first == NULL)
    {
        return 0;
    }
    *first = '\0';
    second = strchr(first + 1, '|');
    if (second == NULL)
    {
        return 0;
    }
    *second = '\0';
    if (strchr(second + 1, '|') != NULL)
    {
        return 0;
    }
    if ((strlen(line) >= sizeof(employee->id)) ||
        (strlen(first + 1) >= sizeof(employee->name)) ||
        (strlen(second + 1) >= sizeof(employee->resume_path)))
    {
        return 0;
    }
    (void)strcpy(employee->id, line);
    (void)strcpy(employee->name, first + 1);
    (void)strcpy(employee->resume_path, second + 1);
    return employee_is_valid(employee);
}

int employee_list(const char *database_path, Employee employees[], size_t capacity, size_t *count)
{
    FILE *database;
    char line[EMPLOYEE_LINE_CAPACITY];
    size_t used = 0U;

    if ((database_path == NULL) || (employees == NULL) || (count == NULL) || (capacity == 0U))
    {
        return EMPLOYEE_INVALID;
    }
    *count = 0U;
    database = fopen(database_path, "r");
    if (database == NULL)
    {
        if (errno == ENOENT)
        {
            return EMPLOYEE_OK;
        }
        return EMPLOYEE_IO_ERROR;
    }
    while (fgets(line, (int)sizeof(line), database) != NULL)
    {
        if ((used >= capacity) || (parse_line(line, &employees[used]) == 0))
        {
            (void)fclose(database);
            return EMPLOYEE_INVALID;
        }
        ++used;
    }
    if (ferror(database) != 0)
    {
        (void)fclose(database);
        return EMPLOYEE_IO_ERROR;
    }
    if (fclose(database) != 0)
    {
        return EMPLOYEE_IO_ERROR;
    }
    *count = used;
    return EMPLOYEE_OK;
}

int employee_find(const char *database_path, const char *employee_id, Employee *employee)
{
    Employee employees[MAX_EMPLOYEES];
    size_t count;
    size_t index;
    int status;

    if ((employee_id == NULL) || (employee == NULL))
    {
        return EMPLOYEE_INVALID;
    }
    status = employee_list(database_path, employees, MAX_EMPLOYEES, &count);
    if (status != EMPLOYEE_OK)
    {
        return status;
    }
    for (index = 0U; index < count; ++index)
    {
        if (strcmp(employees[index].id, employee_id) == 0)
        {
            *employee = employees[index];
            return EMPLOYEE_OK;
        }
    }
    return EMPLOYEE_NOT_FOUND;
}

int employee_add(const char *database_path, const Employee *employee)
{
    Employee employees[MAX_EMPLOYEES];
    size_t count;
    size_t index;
    FILE *database;
    int status;

    if ((database_path == NULL) || (employee_is_valid(employee) == 0))
    {
        return EMPLOYEE_INVALID;
    }
    status = employee_list(database_path, employees, MAX_EMPLOYEES, &count);
    if (status != EMPLOYEE_OK)
    {
        return status;
    }
    if (count >= MAX_EMPLOYEES)
    {
        return EMPLOYEE_FULL;
    }
    for (index = 0U; index < count; ++index)
    {
        if (strcmp(employees[index].id, employee->id) == 0)
        {
            return EMPLOYEE_DUPLICATE;
        }
    }
    database = fopen(database_path, "a");
    if (database == NULL)
    {
        return EMPLOYEE_IO_ERROR;
    }
    if (fprintf(database, "%s|%s|%s\n", employee->id, employee->name, employee->resume_path) < 0)
    {
        (void)fclose(database);
        return EMPLOYEE_IO_ERROR;
    }
    return (fclose(database) == 0) ? EMPLOYEE_OK : EMPLOYEE_IO_ERROR;
}

int employee_remove(const char *database_path, const char *employee_id)
{
    Employee employees[MAX_EMPLOYEES];
    size_t count;
    size_t index;
    FILE *temporary;
    char temporary_path[512];
    int found = 0;
    int status;

    if ((database_path == NULL) || (employee_id == NULL) || (employee_id[0] == '\0') ||
        (snprintf(temporary_path, sizeof(temporary_path), "%s.tmp", database_path) < 0) ||
        (strlen(database_path) + 5U >= sizeof(temporary_path)))
    {
        return EMPLOYEE_INVALID;
    }
    status = employee_list(database_path, employees, MAX_EMPLOYEES, &count);
    if (status != EMPLOYEE_OK)
    {
        return status;
    }
    temporary = fopen(temporary_path, "w");
    if (temporary == NULL)
    {
        return EMPLOYEE_IO_ERROR;
    }
    for (index = 0U; index < count; ++index)
    {
        if (strcmp(employees[index].id, employee_id) == 0)
        {
            found = 1;
        }
        else if (fprintf(temporary, "%s|%s|%s\n", employees[index].id,
                         employees[index].name, employees[index].resume_path) < 0)
        {
            (void)fclose(temporary);
            (void)remove(temporary_path);
            return EMPLOYEE_IO_ERROR;
        }
    }
    if ((fclose(temporary) != 0) || (found == 0))
    {
        (void)remove(temporary_path);
        return (found == 0) ? EMPLOYEE_NOT_FOUND : EMPLOYEE_IO_ERROR;
    }
    status = EMPLOYEE_IO_ERROR;
#if defined(_WIN32)
    if (MoveFileExA(temporary_path, database_path,
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0)
    {
        status = EMPLOYEE_OK;
    }
#else
    if (rename(temporary_path, database_path) == 0)
    {
        status = EMPLOYEE_OK;
    }
#endif
    (void)remove(temporary_path);
    return status;
}

int employee_set_resume(const char *database_path, const char *employee_id, const char *resume_path)
{
    Employee employees[MAX_EMPLOYEES];
    size_t count;
    size_t index;
    FILE *temporary;
    char temporary_path[512];
    int found = 0;
    int status;

    if ((database_path == NULL) || (employee_id == NULL) || (employee_id[0] == '\0') ||
        (resume_path == NULL) || (resume_path[0] == '\0') ||
        (strlen(resume_path) >= EMPLOYEE_RESUME_PATH_CAPACITY) ||
        (strchr(resume_path, '|') != NULL) || (strchr(resume_path, '\n') != NULL) ||
        (strchr(resume_path, '\r') != NULL) ||
        (snprintf(temporary_path, sizeof(temporary_path), "%s.tmp", database_path) < 0) ||
        (strlen(database_path) + 5U >= sizeof(temporary_path)))
    {
        return EMPLOYEE_INVALID;
    }
    status = employee_list(database_path, employees, MAX_EMPLOYEES, &count);
    if (status != EMPLOYEE_OK)
    {
        return status;
    }
    for (index = 0U; index < count; ++index)
    {
        if (strcmp(employees[index].id, employee_id) == 0)
        {
            (void)strcpy(employees[index].resume_path, resume_path);
            found = 1;
            break;
        }
    }
    if (found == 0)
    {
        return EMPLOYEE_NOT_FOUND;
    }
    temporary = fopen(temporary_path, "w");
    if (temporary == NULL)
    {
        return EMPLOYEE_IO_ERROR;
    }
    for (index = 0U; index < count; ++index)
    {
        if (fprintf(temporary, "%s|%s|%s\n", employees[index].id,
                    employees[index].name, employees[index].resume_path) < 0)
        {
            (void)fclose(temporary);
            (void)remove(temporary_path);
            return EMPLOYEE_IO_ERROR;
        }
    }
    if (fclose(temporary) != 0)
    {
        (void)remove(temporary_path);
        return EMPLOYEE_IO_ERROR;
    }
#if defined(_WIN32)
    status = (MoveFileExA(temporary_path, database_path,
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0)
                 ? EMPLOYEE_OK : EMPLOYEE_IO_ERROR;
#else
    status = (rename(temporary_path, database_path) == 0) ? EMPLOYEE_OK : EMPLOYEE_IO_ERROR;
#endif
    if (status != EMPLOYEE_OK)
    {
        (void)remove(temporary_path);
    }
    return status;
}
