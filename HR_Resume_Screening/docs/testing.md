# Test plan and test inventory

Run `test.bat` on Windows or `make test` where GNU Make is available. The runners compile with strict GCC warnings, execute the resume/profile/report and employee roster unit-test programs, then exercise CLI help/version, employee add/list/screen/remove, and a complete custom-profile report export.

## Module 1 tests

`test/test_module1.c` directly checks word-character classification and bounded normalization, then exercises the reader with case differences, punctuation, repeated skills, a substring non-match, null/empty/missing paths, empty input, exactly 5 MiB, and one byte over the limit. It also calls `analyze_resume` to confirm orchestration through the public module interface.

## Module 2 tests

`test/test_module2.c` checks default initialization, custom skill addition, case-insensitive duplicate handling, invalid names, the 32-skill ceiling, comments/blank lines/profile deduplication, missing/empty/invalid/oversized profiles, invalid analysis inputs, matched-skill counts, exact score thresholds, report content, and null report arguments.

## CLI integration checks

The Windows and shell test runners build the app, check the unit programs, analyze the sample resume with the sample role profile, export a report, and check that the report contains a Strong match. They also invoke employee add/list/screen/remove, help/version, invalid options, missing paths, empty input, invalid profile, conflicting input/output, and existing-report protection.

## Known verification gaps

The repository does not yet include CI, coverage instrumentation, fuzzing, sanitizer runs, Cppcheck output, or a formal MISRA report. The 80–90% coverage goal from the supplied spreadsheet must be measured and demonstrated before claiming it has been met. Run the complete suite on Linux before declaring Linux support verified.
