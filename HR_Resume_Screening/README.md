# HR Resume Screening System

**C11 console tool for local employee record management and transparent, rule-based resume keyword review.**

This release maintains a small local employee roster and screens a saved employee resume. It reads plain-text resumes, counts whole-word matches from a bounded skill profile, calculates equal-weight skill coverage, and prints a report. Use the built-in profile (C, Java, Python, SQL, Linux) or provide a custom role profile. It is not an automated hiring decision system.

## Quick start on Windows

1. Install GCC and make sure `gcc` is on `PATH`.
2. Run `build.bat` from this folder.
3. Analyze the sample: `build\resume_screen.exe sample\resume.txt`.
4. For a role-specific report: `build\resume_screen.exe --skills sample\skills_software.txt sample\resume.txt --output report.txt`.
5. Run `test.bat` to execute unit and command-line integration tests.

## Linux / Make

```sh
make
./build/resume_screen sample/resume.txt
./build/resume_screen --skills sample/skills_software.txt sample/resume.txt --output report.txt
make test
```

## Command-line reference

```text
resume_screen [options] <resume.txt>
  -h, --help              Show help
  -V, --version           Show version
  -s, --skills <file>     Load role skills, one skill per line
  -o, --output <file>     Create a new text report; existing files are not overwritten
  --                      Stop parsing options (use before a path beginning with a dash)
```

The resume path is required. The output report path must be new. If it already exists, the program stops rather than overwriting it.

## Employee management

Employee records are stored in `data/employees.txt` as local pipe-separated text. IDs are unique and may contain letters, digits, hyphens, and underscores. Quote names containing spaces. A resume path is optional when adding an employee.

```text
build\resume_screen.exe employee add E1001 "Alex Morgan" sample\resume.txt
build\resume_screen.exe employee add E1002 "Jordan Lee"
build\resume_screen.exe employee list
build\resume_screen.exe employee set-resume E1002 sample\resume.txt
build\resume_screen.exe employee screen E1001 --skills sample\skills_software.txt --output E1001_report.txt
build\resume_screen.exe employee remove E1001
```

Use `employee set-resume <id> <resume-path>` to attach or change a resume later. Removing an employee record does not delete the referenced resume. This POC has no login, access permissions, encryption, multi-user locking, audit history, backup, or retention automation. Use synthetic records for demos until your organization adds and reviews appropriate protections.

## Skill profile format

Use one ASCII alphanumeric skill per line, such as `Python`, `Linux`, or `REST`. Blank lines and lines whose first non-space character is `#` are ignored. Matching is case-insensitive, so duplicate names in different cases collapse to one entry. Names with spaces or punctuation (for example `C++`) are rejected. Maximum profile: 32 unique skills, 31 characters each, 16 KiB total.

## Matching and score

Resume text is streamed one time and split into alphanumeric tokens. Punctuation acts as a separator; `Pythonista` does not match `Python`. Every distinct configured skill has equal weight: matched skills ÷ configured skills × 100. Repeated mentions change the frequency count, not the coverage score. Scores of 80% or more are Strong, 50% to below 80% are Potential, and below 50% are Low.

The score does not measure skill proficiency, context, accuracy, seniority, or overall candidate quality. A human reviewer must verify evidence and make decisions.

## Limits, privacy, and deployment

- Resume maximum: 5 MiB. Skill-profile maximum: 16 KiB.
- Uses local files only; no network service or database. Resume contents are not copied into the employee roster.
- Optional reports contain skill names and counts. Save them only in approved locations and follow your retention rules.
- Supports plain-text input only. PDF/DOCX/OCR, GUI, authentication/RBAC, audit logging, encryption, multi-user updates, and semantic synonyms are out of scope.
- GCC strict-warning build and unit/CLI tests are included. Cppcheck, coverage measurement, sanitizer/fuzz tests, formal MISRA C analysis, and Linux release verification remain release-gate work. See `docs/requirements.md`.

## Directory structure

```text
HR_Resume_Screening/
├── include/        Shared limits, types, and module interfaces
├── src/            CLI, file scanning, profiles/reports, employee management, text helpers
├── test/           Module unit tests and CLI integration checks
├── data/           Local employee roster; protect this candidate data
├── sample/         Example resume and configurable skill profile
├── docs/           Requirements, architecture, deployment, and testing
├── build/          Generated executable, test binaries, and temporary reports
├── Makefile        GCC/Make build and test entry points
├── build.bat       Windows build entry point
└── test.bat        Windows unit and CLI integration test runner
```

A detailed beginner guide with line-by-line code explanations is available at [docs/HR_Resume_Screening_v1.2_Beginner_Guide.docx](docs/HR_Resume_Screening_v1.2_Beginner_Guide.docx).
