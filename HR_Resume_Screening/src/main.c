#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

#include "module2.h"
#include "employee.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <io.h>
#include <sys/stat.h>
extern FILE *_fdopen(int descriptor, const char *mode);
#else
#include <unistd.h>
#endif

#define APPLICATION_VERSION "1.2.0"
#define EMPLOYEE_DATABASE_PATH "data/employees.txt"

static int print_analysis_error(int status, const char *path);
static FILE *open_new_report(const char *path);

static void print_usage(const char *program)
{
    (void)printf("HR Resume Screening System v%s\n\n", APPLICATION_VERSION);
    (void)printf("Usage: %s [options] <resume.txt>\n\n", program);
    (void)printf("       %s employee <add|remove|list|screen> ...\n\n", program);
    (void)printf("Options:\n");
    (void)printf("  -h, --help              Show this help\n");
    (void)printf("  -V, --version           Show version\n");
    (void)printf("  -s, --skills <file>     Load a one-skill-per-line profile\n");
    (void)printf("  -o, --output <file>     Create a new plain-text report file\n\n");
    (void)printf("Example:\n  %s --skills sample/skills_software.txt sample/resume.txt\n", program);
    (void)printf("  %s sample/resume.txt --output report.txt\n", program);
    (void)printf("  %s employee add E1001 \"Alex Morgan\" sample/resume.txt\n", program);
    (void)printf("  %s employee screen E1001\n", program);
}

static int employee_command(int argument_count, char *arguments[])
{
    Employee employee;
    Employee employees[MAX_EMPLOYEES];
    size_t count = 0U;
    size_t index;
    int status;

    if ((argument_count < 3) || (strcmp(arguments[2], "--help") == 0))
    {
        (void)printf("Usage: %s employee add <id> <name> [resume.txt]\n", arguments[0]);
        (void)printf("       %s employee remove <id>\n", arguments[0]);
        (void)printf("       %s employee set-resume <id> <resume.txt>\n", arguments[0]);
        (void)printf("       %s employee list\n", arguments[0]);
        (void)printf("       %s employee screen <id> [--skills file] [--output report.txt]\n", arguments[0]);
        (void)printf("Employee records are stored locally in %s\n", EMPLOYEE_DATABASE_PATH);
        return (argument_count < 3) ? 2 : 0;
    }
    if (strcmp(arguments[2], "add") == 0)
    {
        if ((argument_count != 5) && (argument_count != 6))
        {
            (void)fprintf(stderr, "Error: add requires an ID, a quoted name, and an optional resume path.\n");
            return 2;
        }
        (void)memset(&employee, 0, sizeof(employee));
        if ((strlen(arguments[3]) >= sizeof(employee.id)) ||
            (strlen(arguments[4]) >= sizeof(employee.name)) ||
            ((argument_count == 6) && (strlen(arguments[5]) >= sizeof(employee.resume_path))))
        {
            (void)fprintf(stderr, "Error: employee field is longer than its supported limit.\n");
            return 2;
        }
        (void)strcpy(employee.id, arguments[3]);
        (void)strcpy(employee.name, arguments[4]);
        if (argument_count == 6)
        {
            (void)strcpy(employee.resume_path, arguments[5]);
        }
        status = employee_add(EMPLOYEE_DATABASE_PATH, &employee);
        if (status != EMPLOYEE_OK)
        {
            (void)fprintf(stderr, "Error: employee could not be added (status %d).\n", status);
            return 1;
        }
        (void)printf("Added employee %s (%s).\n", employee.name, employee.id);
        return 0;
    }
    if (strcmp(arguments[2], "remove") == 0)
    {
        if (argument_count != 4)
        {
            (void)fprintf(stderr, "Error: remove requires one employee ID.\n");
            return 2;
        }
        status = employee_remove(EMPLOYEE_DATABASE_PATH, arguments[3]);
        if (status != EMPLOYEE_OK)
        {
            (void)fprintf(stderr, "Error: employee could not be removed (status %d).\n", status);
            return 1;
        }
        (void)printf("Removed employee %s. The resume file was not deleted.\n", arguments[3]);
        return 0;
    }
    if (strcmp(arguments[2], "set-resume") == 0)
    {
        if (argument_count != 5)
        {
            (void)fprintf(stderr, "Error: set-resume requires an employee ID and resume path.\n");
            return 2;
        }
        status = employee_set_resume(EMPLOYEE_DATABASE_PATH, arguments[3], arguments[4]);
        if (status != EMPLOYEE_OK)
        {
            (void)fprintf(stderr, "Error: resume path could not be saved (status %d).\n", status);
            return 1;
        }
        (void)printf("Saved resume path for employee %s.\n", arguments[3]);
        return 0;
    }
    if (strcmp(arguments[2], "list") == 0)
    {
        if (argument_count != 3)
        {
            (void)fprintf(stderr, "Error: list takes no additional arguments.\n");
            return 2;
        }
        status = employee_list(EMPLOYEE_DATABASE_PATH, employees, MAX_EMPLOYEES, &count);
        if (status != EMPLOYEE_OK)
        {
            (void)fprintf(stderr, "Error: employee database could not be read (status %d).\n", status);
            return 1;
        }
        (void)printf("Employee ID | Name | Resume path\n");
        for (index = 0U; index < count; ++index)
        {
            (void)printf("%s | %s | %s\n", employees[index].id, employees[index].name,
                         employees[index].resume_path);
        }
        return 0;
    }
    if (strcmp(arguments[2], "screen") == 0)
    {
        const char *skills_path = NULL;
        const char *report_path = NULL;
        AnalysisResult result;
        FILE *report;
        int argument_index;

        if ((argument_count < 4) || (employee_find(EMPLOYEE_DATABASE_PATH, arguments[3], &employee) != EMPLOYEE_OK))
        {
            (void)fprintf(stderr, "Error: employee was not found or the database is unavailable.\n");
            return 1;
        }
        if (employee.resume_path[0] == '\0')
        {
            (void)fprintf(stderr, "Error: this employee does not have a resume path saved.\n");
            return 1;
        }
        for (argument_index = 4; argument_index < argument_count; ++argument_index)
        {
            if ((strcmp(arguments[argument_index], "--skills") == 0) && ((argument_index + 1) < argument_count))
            {
                skills_path = arguments[++argument_index];
            }
            else if ((strcmp(arguments[argument_index], "--output") == 0) && ((argument_index + 1) < argument_count))
            {
                report_path = arguments[++argument_index];
            }
            else
            {
                (void)fprintf(stderr, "Error: invalid employee screen option.\n");
                return 2;
            }
        }
        if (skills_path == NULL)
        {
            initialize_result(&result);
            status = ANALYSIS_OK;
        }
        else
        {
            status = load_skill_profile(skills_path, &result);
        }
        if (status == ANALYSIS_OK)
        {
            status = analyze_resume(employee.resume_path, &result);
        }
        if (status != ANALYSIS_OK)
        {
            return print_analysis_error(status, employee.resume_path);
        }
        (void)printf("Employee: %s (%s)\n", employee.name, employee.id);
        if (write_report(stdout, &result) != 0)
        {
            return 1;
        }
        if (report_path != NULL)
        {
            report = open_new_report(report_path);
            if (report == NULL)
            {
                (void)fprintf(stderr, "Error: cannot create report '%s'.\n", report_path);
                return 1;
            }
            status = write_report(report, &result);
            if ((fclose(report) != 0) || (status != 0))
            {
                (void)fprintf(stderr, "Error: report could not be written completely.\n");
                return 1;
            }
        }
        return 0;
    }
    (void)fprintf(stderr, "Error: unknown employee action '%s'.\n", arguments[2]);
    return 2;
}

static int print_analysis_error(int status, const char *path)
{
    if (status == ANALYSIS_EMPTY)
    {
        (void)fprintf(stderr, "Error: resume file '%s' is empty.\n", path);
    }
    else if (status == ANALYSIS_RESUME_TOO_LARGE)
    {
        (void)fprintf(stderr, "Error: resume exceeds the 5 MiB limit.\n");
    }
    else if (status == ANALYSIS_PROFILE_TOO_LARGE)
    {
        (void)fprintf(stderr, "Error: skill profile exceeds the 16 KiB limit.\n");
    }
    else if (status == ANALYSIS_INVALID_PROFILE)
    {
        (void)fprintf(stderr, "Error: skill profile is empty or contains an invalid entry.\n");
    }
    else if (status == ANALYSIS_INVALID_INPUT)
    {
        (void)fprintf(stderr, "Error: required input is missing or invalid.\n");
    }
    else
    {
        (void)fprintf(stderr, "Error: could not read '%s'. Check the path and permissions.\n", path);
    }
    return 1;
}

static FILE *open_new_report(const char *path)
{
    int descriptor;
    FILE *stream;

#if defined(_WIN32)
    descriptor = _open(path, _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
    if (descriptor < 0)
    {
        return NULL;
    }
    stream = _fdopen(descriptor, "wb");
    if (stream == NULL)
    {
        (void)_close(descriptor);
    }
#else
    descriptor = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (descriptor < 0)
    {
        return NULL;
    }
    stream = fdopen(descriptor, "wb");
    if (stream == NULL)
    {
        (void)close(descriptor);
    }
#endif
    return stream;
}

int main(int argument_count, char *arguments[])
{
    const char *resume_path = NULL;
    const char *report_path = NULL;
    const char *skills_path = NULL;
    AnalysisResult result;
    int status;
    int index;
    int options_enabled = 1;

    if ((argument_count >= 2) && (strcmp(arguments[1], "employee") == 0))
    {
        return employee_command(argument_count, arguments);
    }

    for (index = 1; index < argument_count; ++index)
    {
        if ((options_enabled != 0) && (strcmp(arguments[index], "--") == 0))
        {
            options_enabled = 0;
        }
        else if ((options_enabled != 0) &&
                 ((strcmp(arguments[index], "--help") == 0) ||
                  (strcmp(arguments[index], "-h") == 0)))
        {
            print_usage(arguments[0]);
            return 0;
        }
        else if ((options_enabled != 0) &&
                 ((strcmp(arguments[index], "--version") == 0) ||
                  (strcmp(arguments[index], "-V") == 0)))
        {
            (void)printf("HR Resume Screening System %s\n", APPLICATION_VERSION);
            return 0;
        }
        else if ((options_enabled != 0) &&
                 ((strcmp(arguments[index], "--output") == 0) ||
                  (strcmp(arguments[index], "-o") == 0)))
        {
            if (((index + 1) >= argument_count) || (arguments[index + 1][0] == '\0') ||
                ((options_enabled != 0) && (arguments[index + 1][0] == '-')))
            {
                (void)fprintf(stderr, "Error: --output requires a non-empty file path.\n");
                return 2;
            }
            report_path = arguments[++index];
        }
        else if ((options_enabled != 0) &&
                 ((strcmp(arguments[index], "--skills") == 0) ||
                  (strcmp(arguments[index], "-s") == 0)))
        {
            if (((index + 1) >= argument_count) || (arguments[index + 1][0] == '\0') ||
                ((options_enabled != 0) && (arguments[index + 1][0] == '-')))
            {
                (void)fprintf(stderr, "Error: --skills requires a non-empty file path.\n");
                return 2;
            }
            skills_path = arguments[++index];
        }
        else if ((options_enabled != 0) && (arguments[index][0] == '-'))
        {
            (void)fprintf(stderr, "Error: unknown option '%s'. Use --help.\n", arguments[index]);
            return 2;
        }
        else if (resume_path == NULL)
        {
            resume_path = arguments[index];
        }
        else
        {
            (void)fprintf(stderr, "Error: provide exactly one resume file.\n");
            return 2;
        }
    }

    if (resume_path == NULL)
    {
        print_usage(arguments[0]);
        return 2;
    }
    if ((report_path != NULL) && (strcmp(report_path, resume_path) == 0))
    {
        (void)fprintf(stderr, "Error: report path must not be the resume path.\n");
        return 2;
    }
    if ((report_path != NULL) && (skills_path != NULL) &&
        (strcmp(report_path, skills_path) == 0))
    {
        (void)fprintf(stderr, "Error: report path must not be the skills profile path.\n");
        return 2;
    }

    if (skills_path == NULL)
    {
        initialize_result(&result);
        status = ANALYSIS_OK;
    }
    else
    {
        status = load_skill_profile(skills_path, &result);
    }
    if (status != ANALYSIS_OK)
    {
        return print_analysis_error(status, skills_path);
    }
    status = analyze_resume(resume_path, &result);
    if (status != ANALYSIS_OK)
    {
        return print_analysis_error(status, resume_path);
    }
    if (write_report(stdout, &result) != 0)
    {
        (void)fprintf(stderr, "Error: could not write report to the console.\n");
        return 1;
    }
    if (report_path != NULL)
    {
        FILE *report = open_new_report(report_path);
        if (report == NULL)
        {
            (void)fprintf(stderr, "Error: cannot create report '%s'; it may already exist.\n", report_path);
            return 1;
        }
        status = write_report(report, &result);
        if (fclose(report) != 0)
        {
            status = -1;
        }
        if (status != 0)
        {
            (void)fprintf(stderr, "Error: report could not be written completely.\n");
            return 1;
        }
    }
    return 0;
}
