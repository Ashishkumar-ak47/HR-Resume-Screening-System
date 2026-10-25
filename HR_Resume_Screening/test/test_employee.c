#include "employee.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *database_path = "build/test_employees.txt";
    Employee employee;
    Employee found;
    Employee employees[MAX_EMPLOYEES];
    size_t count = 0U;

    (void)remove(database_path);
    (void)remove("build/test_employees.txt.tmp");
    assert(employee_list(database_path, employees, MAX_EMPLOYEES, &count) == EMPLOYEE_OK);
    assert(count == 0U);
    (void)memset(&employee, 0, sizeof(employee));
    (void)strcpy(employee.id, "EMP-100");
    (void)strcpy(employee.name, "Alex Morgan");
    employee.resume_path[0] = '\0';
    assert(employee_is_valid(&employee) == 1);
    assert(employee_add(database_path, &employee) == EMPLOYEE_OK);

    assert(employee_add(database_path, &employee) == EMPLOYEE_DUPLICATE);
    assert(employee_find(database_path, "EMP-100", &found) == EMPLOYEE_OK);
    assert(found.resume_path[0] == '\0');
    assert(employee_set_resume(database_path, "EMP-100", "sample/second_resume.txt") == EMPLOYEE_OK);
    assert(employee_find(database_path, "EMP-100", &found) == EMPLOYEE_OK);
    assert(strcmp(found.name, "Alex Morgan") == 0);
    assert(strcmp(found.resume_path, "sample/second_resume.txt") == 0);
    assert(employee_list(database_path, employees, MAX_EMPLOYEES, &count) == EMPLOYEE_OK);
    assert(count == 1U);
    assert(employee_find(database_path, "NOPE", &found) == EMPLOYEE_NOT_FOUND);
    assert(employee_set_resume(database_path, "NOPE", "sample/resume.txt") == EMPLOYEE_NOT_FOUND);
    assert(employee_remove(database_path, "NOPE") == EMPLOYEE_NOT_FOUND);
    assert(employee_remove(database_path, "EMP-100") == EMPLOYEE_OK);
    assert(employee_list(database_path, employees, MAX_EMPLOYEES, &count) == EMPLOYEE_OK);
    assert(count == 0U);
    assert(employee_remove(database_path, "EMP-100") == EMPLOYEE_NOT_FOUND);

    (void)strcpy(employee.id, "BAD|ID");
    assert(employee_is_valid(&employee) == 0);
    (void)strcpy(employee.id, "EMP-101");
    (void)strcpy(employee.name, "Bad|Name");
    assert(employee_is_valid(&employee) == 0);
    (void)strcpy(employee.name, "No Resume Yet");
    employee.resume_path[0] = '\0';
    assert(employee_is_valid(&employee) == 1);
    (void)remove(database_path);
    (void)remove("build/test_employees.txt.tmp");
    (void)puts("test_employee: all checks passed");
    return 0;
}
