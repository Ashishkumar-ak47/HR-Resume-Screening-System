# Changelog

## 1.2.0

- Added local employee records with add, list, find, and remove operations.
- Added employee resume screening with role-specific profiles and report output.
- Allowed employee records to be created before a resume is available.
- Added a command to attach or change an employee's resume path later.
- Added roster field validation, duplicate ID handling, replacement-file removal, and dedicated employee tests.
- Documented the new candidate-data storage and its production security gaps.

## 1.1.0

- Added validated role-specific skill profiles with fixed memory and file-size limits.
- Raised the resume input limit to 5 MiB and report creation now refuses to overwrite an existing file.
- Made scoring and classification separate functions; expanded module tests and added CLI integration checks.
- Added a deployment-oriented requirements baseline, architecture, deployment, and testing guides.
- Clarified the POC's hiring, privacy, and compliance limits.

## 1.0.0

- Added command-line help/version flags and text report export.
- Improved report layout with coverage, matches, missing skills, frequency, and an explicit human-review note.
- Added Windows build/test scripts and sample resume.
- Expanded tests for whole-word matching, case, punctuation, empty/missing input, and the 1 MiB size boundary.
