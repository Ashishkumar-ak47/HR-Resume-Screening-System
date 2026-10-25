# Deployment guide

## Prerequisites

- GCC with C11 support.
- Windows Command Prompt/PowerShell with GCC on `PATH`, or Linux with GCC and Make.
- A plain-text resume and, optionally, a role profile using one ASCII alphanumeric skill per line.

## Build

Windows: run `build.bat`. Linux/Make: run `make`. Build output is `build/resume_screen.exe` on Windows or `build/resume_screen` on Make-based systems.

## Run

```text
build\resume_screen.exe sample\resume.txt
build\resume_screen.exe --skills sample\skills_software.txt sample\resume.txt
build\resume_screen.exe --skills sample\skills_software.txt sample\resume.txt --output report.txt
build\resume_screen.exe --help
```

On Linux, replace Windows path separators and executable suffix as appropriate. If a path begins with a dash, place `--` before it. The output report must be a new path; remove or rename an old report yourself before rerunning.

## Data handling

The app reads files from the local machine only. It does not persist the resume or use a database/network. An explicitly requested output report contains skill counts and coverage; store it only in an approved location and delete it under your retention policy.

## Employee roster commands

Use `employee add <id> <name> [resume-path]`, `employee set-resume <id> <resume-path>`, `employee list`, `employee screen <id> [--skills file] [--output report]`, and `employee remove <id>`. Quote names with spaces. A record can be created before a resume is available and attached later. Records persist in `data/employees.txt` with a maximum of 500 employees. Removing a record does not delete the resume file.

The roster is plain text and has no encryption, access control, audit history, backups, retention automation, or concurrent-user protection. Do not use real candidate data until your organization provides those safeguards and reviews the storage location.

## Operational cautions

Do not use the POC score as the sole basis for screening. Inspect context and verify skills with a human reviewer. The current parser only supports plain-text input and exact single-word matches. Before production deployment, complete the release gate in `requirements.md` and establish ownership for security updates, test automation, and support.
