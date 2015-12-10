### mostly stolen from http://eigenstate.org/notes/makefiles

CC = clang
SHELL = /bin/bash

CFLAGS += -std=c11 -pedantic -Werror -Wall -Wextra -Isrc
DEV_CFLAGS = -O0 -ggdb

SOURCES=$(wildcard src/**/*.c src/*.c)
LIBOBJ=$(patsubst %.c,%.o,$(SOURCES))

TEST_SOURCES=$(wildcard tests/**/*.c tests/*.c)
TESTS=$(patsubst %.c,%,$(TEST_SOURCES))

# name of output library
LIB=mystuff

all: build build/lib$(LIB).a build/lib$(LIB).so check

all-dev: CFLAGS += $(DEV_CFLAGS)
all-dev: all

%.o: %.c .deps
	$(CC) $(CFLAGS) -c -o $@ $< -MMD -MP -MF .deps/$*.d

# binaries (test files)
%: %.c .deps build/lib$(LIB).so
	$(CC) $(CFLAGS) -o $@ $< build/lib$(LIB).so -MMD -MP -MF .deps/$*.d

build/lib$(LIB).a: $(LIBOBJ)
	$(AR) -rcs $@ $(LIBOBJ)
	ranlib $@

build/lib$(LIB).so: $(LIBOBJ)
	$(CC) -shared -o $@ $(LIBOBJ)

check: $(TESTS)
	for name in $(TESTS); do ./$$name; done

check-dev: CFLAGS += -O0 -ggdb
check-dev: check

build:
	mkdir -p build

clean:
	rm -rf .deps
	rm -rf build
	find ./tests -name "*_test" -delete
	find . \( -name "*.o" -o -name "*.a" -o -name "*.dSYM" \) -delete

.SUFFIXES:
.SECONDARY:

.deps:
	mkdir -p .deps/src
	mkdir -p .deps/tests

# # subtle: directories are listed as changed when entries are
# # created, leading to spurious rebuilds.
# .deps/stamp:
# 	mkdir -p .deps/src && touch/stamp

-include .deps/src/*.d
-include .deps/tests/*.d
