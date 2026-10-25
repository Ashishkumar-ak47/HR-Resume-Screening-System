#!/bin/sh
set -eu

APP=./build/resume_screen
REPORT=build/test_report.txt

rm -f "$REPORT" build/test_help.txt build/test_version.txt build/empty_resume.txt build/invalid_skills.txt
"$APP" --help > build/test_help.txt
"$APP" --version > build/test_version.txt
grep -q "Usage:" build/test_help.txt
grep -q "1.2.0" build/test_version.txt

if "$APP" --unknown > /dev/null 2>&1; then
    echo "unknown option unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 2 ]
fi
if "$APP" --skills > /dev/null 2>&1; then
    echo "missing skill-profile path unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 2 ]
fi
if "$APP" > /dev/null 2>&1; then
    echo "missing resume path unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 2 ]
fi

"$APP" --skills sample/skills_software.txt sample/resume.txt --output "$REPORT" > /dev/null
grep -q "Strong match" "$REPORT"
cp "$REPORT" build/test_report_before.txt
if "$APP" sample/resume.txt --output "$REPORT" > /dev/null 2>&1; then
    echo "existing report was unexpectedly overwritten" >&2
    exit 1
else
    [ "$?" -eq 1 ]
fi
cmp -s "$REPORT" build/test_report_before.txt

"$APP" employee add T100 "Test Employee" sample/resume.txt
"$APP" employee list | grep -q "Test Employee"
"$APP" employee set-resume T100 sample/resume.txt
"$APP" employee screen T100 > /dev/null
"$APP" employee remove T100

: > build/empty_resume.txt
if "$APP" build/empty_resume.txt > /dev/null 2>&1; then
    echo "empty resume unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 1 ]
fi
printf 'C++\n' > build/invalid_skills.txt
if "$APP" --skills build/invalid_skills.txt sample/resume.txt > /dev/null 2>&1; then
    echo "invalid profile unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 1 ]
fi
if "$APP" sample/resume.txt --output sample/resume.txt > /dev/null 2>&1; then
    echo "conflicting input/output path unexpectedly succeeded" >&2
    exit 1
else
    [ "$?" -eq 2 ]
fi

rm -f "$REPORT" build/test_report_before.txt build/test_help.txt build/test_version.txt build/empty_resume.txt build/invalid_skills.txt
echo "test_cli: all integration checks passed"
