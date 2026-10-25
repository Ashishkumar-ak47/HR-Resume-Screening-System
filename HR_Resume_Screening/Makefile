CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Iinclude
SOURCES = src/main.c src/module1.c src/module2.c src/utils.c src/employee.c
CORE_SOURCES = src/module1.c src/module2.c src/utils.c src/employee.c

.PHONY: all clean test run

all: build/resume_screen

build:
	mkdir -p build

build/resume_screen: $(SOURCES) | build
	$(CC) $(CFLAGS) $(SOURCES) -o $@

build/test_module1: test/test_module1.c $(CORE_SOURCES) | build
	$(CC) $(CFLAGS) $^ -o $@

build/test_module2: test/test_module2.c $(CORE_SOURCES) | build
	$(CC) $(CFLAGS) $^ -o $@

build/test_employee: test/test_employee.c src/employee.c | build
	$(CC) $(CFLAGS) $^ -o $@

test: build/test_module1 build/test_module2 build/test_employee build/resume_screen
	./build/test_module1
	./build/test_module2
	./build/test_employee
	sh test/test_cli.sh

run: build/resume_screen
	./build/resume_screen sample/resume.txt

clean:
	rm -f build/resume_screen build/test_module1 build/test_module2 build/test_employee build/test_report.txt
