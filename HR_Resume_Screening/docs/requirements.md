# Deployment-oriented requirements baseline

## Purpose and boundaries

The application is a local, command-line screening aid. It compares plain-text resume tokens with a role-specific list of single-word skills and produces an auditable frequency and coverage report. It is not an applicant-tracking system, an AI model, a credential verifier, or an automated hiring decision-maker.

## Functional requirements

| ID | Requirement | Acceptance evidence |
| --- | --- | --- |
| FR-01 | Read one UTF-8/ASCII-compatible plain-text resume path from the command line. | A readable, non-empty file up to 5 MiB is analyzed. |
| FR-02 | Use either the built-in five-skill profile or a user-supplied profile file. | `--skills` loads a valid profile; without it, C, Java, Python, SQL, and Linux are used. |
| FR-03 | Parse a profile as one ASCII alphanumeric skill per line; ignore blank lines and lines whose first non-space character is `#`. | Case is normalized, duplicate entries collapse, and malformed/empty profiles fail safely. |
| FR-04 | Limit a profile to 32 unique skills, each at most 31 characters, and a profile file to 16 KiB. | Over-limit and invalid profile tests return explicit errors. |
| FR-05 | Tokenize a resume in one pass, case-insensitively, with punctuation as a separator. | Whole tokens match; `Pythonista` does not match `Python`. |
| FR-06 | Count occurrences and distinct matches for every configured skill. | Unit tests assert repeated counts and matched-skill totals. |
| FR-07 | Calculate equal-weight coverage as matched distinct skills ÷ configured skills × 100. | 80% or more is Strong, 50% to below 80% is Potential, and below 50% is Low. |
| FR-08 | Display the file size, token count, matched skills, skills to review, sorted frequencies, coverage, and classification. | Report output contains all named sections. |
| FR-09 | Optionally save the report as a new plain-text file. | `--output` creates a report and refuses to overwrite an existing path. |
| FR-10 | Offer `--help`, `--version`, short aliases, and `--` to end option parsing. | CLI integration checks confirm these commands complete. |
| FR-11 | Reject invalid/missing paths, empty resumes, malformed/oversized profiles, oversized resumes, conflicting output paths, and invalid option combinations without crashing. | Unit and CLI tests assert documented error status and message paths. |
| FR-12 | Keep resume content local; do not send it to a network service or store it in a database. | Source has no network/database dependency; generated report is only written on explicit request. |
| FR-13 | Add an employee record with a unique ID, name, and optional resume path. | `employee add` validates fields and rejects duplicate IDs. |
| FR-14 | List employees and retrieve a record by ID. | `employee list` displays records; screening locates the selected record. |
| FR-15 | Remove an employee record without deleting its resume file. | `employee remove` replaces the roster using a temporary file; resume files remain untouched. |
| FR-16 | Screen a selected employee's saved resume using shared skill and report logic. | `employee screen <id>` calls the existing profile, analyzer, and report functions. |
| FR-17 | Allow records without a resume and clearly reject screening them until a resume is provided. | Add accepts no resume path; screening reports the missing path. |
| FR-18 | Attach or change an employee's resume path after creating their record. | `employee set-resume <id> <path>` updates the roster through a temporary replacement file. |

## Non-functional requirements

- **Performance:** stream the resume once; maximum resume size 5 MiB; fixed memory use independent of resume size.
- **Reliability:** checked file open/read/close and report-write results; deterministic token and score rules.
- **Maintainability:** modules separate CLI, file scanning, analysis/reporting, and small text helpers; shared limits/types are in `include/common.h`.
- **Portability:** ISO C11-compatible code and GCC build scripts for Windows and Make-based environments. Run platform-specific builds before release on each supported OS.
- **Security:** bounded arrays, no heap allocation, a file-size cap, profile validation, and exclusive report creation. Do not process untrusted input on behalf of a third party without organizational review.
- **Privacy:** process locally; use approved storage for resume/report files; do not retain reports longer than needed.
- **Employee data:** the roster is plain text in `data/employees.txt`, limited to 500 records and not encrypted. Restrict access outside this program and use synthetic data during demonstrations.
- **Accessibility:** this release is text-only and has no GUI accessibility testing; terminal readability should be evaluated in the deployment environment.

## Operating limits and explicit product decisions

- Skill entries are ASCII alphanumeric single words only. Phrases, punctuation-bearing names such as `C++`, synonyms, spelling variants, and semantic matches are not supported.
- Input and profile sizes are bounded as listed above. Report creation will not overwrite an existing file.
- Coverage counts distinct required skills, not proficiency, recency, context, or quality. Repetition affects frequency but not score.
- Strong/Potential/Low thresholds are configurable only by code change in this release and have not been validated against hiring outcomes.
- PDF/DOCX parsing, OCR, database storage, authentication, role-based access control, audit logging, encryption, backup/restore, retention automation, concurrent updates, and a graphical interface are out of scope. This is a local-file employee-management POC, not a production HRIS.
- Human review is mandatory. The software must not make or automatically execute hiring decisions.

## Release gate before organizational deployment

1. Select and document an approved MISRA C edition/ruleset if required; run a configured analyzer and record deviations.
2. Run the full unit and CLI integration suite on each supported OS/compiler; add CI and a measured function/branch coverage report.
3. Complete security, privacy, records-retention, accessibility, and threat-model reviews with the deploying organization.
4. Validate score behavior and false matches on representative, permissioned data; review fairness and human-oversight procedures.
5. Before accepting real candidate data, implement and review authentication, authorization, encryption, audit history, backup/restore, retention/deletion, incident handling, and concurrent-user controls; confirm input/profile/report policies too.
