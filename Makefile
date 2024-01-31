# MIT License
# Copyright (c) 2023 Lauri Lorenzo Fiestas
# https://github.com/PrinssiFiestas/printf/blob/main/LICENSE.md

TARGET_RELEASE = printf.a
TARGET_DEBUG   = printfd.a

# Targets
.PHONY: release		# Optimized build. Default target.
.PHONY: debug		# Non-optimized build with debug symbols.
.PHONY: tests		# Release build. Runs release unit tests.
.PHONY: dtests		# Debug build. Runs debug unit tests.
.PHONY: build_tests	# Build release unit tests.
.PHONY: build_dtests	# Build debug unit tests.
.PHONY: run_tests	# Runs release unit tests without building.
.PHONY: run_dtests	# Runs debug unit tests without building.
.PHONY: clean		# Removes build directory.

release: CFLAGS += -O3
release: build/$(TARGET_RELEASE)

debug: CFLAGS += -ggdb3 -DGP_DEBUG
debug: build/$(TARGET_DEBUG)

build_tests:  CFLAGS += -O3
build_dtests: CFLAGS += -ggdb3 -DGP_DEBUG

# --------------------------------------------------------------------------- #

CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Wdouble-promotion			# Stricter types
CFLAGS += -Wno-missing-field-initializers	# Allow incomplete struct init
CFLAGS += -Wno-comment				# Allow comments with backslash
CFLAGS += -Iinclude

# Enable multithreaded make
NPROC = $(shell echo `nproc`)			# Detect processor count in Bash
THREAD_COUNT = $(if $(NPROC),$(NPROC),4)	# Use 4 threads if not Bash
MAKEFLAGS += -j$(THREAD_COUNT)

# Disable implicit rules
.SUFFIXES:

# Auto-generate header file dependency files
AUTO_GEN_DEPS = -MMD -MP

# --------------------------------------------------------------------------- #

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))

DEBUG_OBJS = $(OBJS:.o=d.o)

ifeq ($(OS), Windows_NT)
	EXE_EXT = .exe
else
	EXE_EXT =
endif

TEST_SRCS = $(wildcard tests/test_*.c)
TEST_EXEC = $(patsubst tests/test_%.c,build/test_%$(EXE_EXT),$(TEST_SRCS))
TEST_DEBUG_EXEC = $(patsubst tests/test_%.c,build/test_%d$(EXE_EXT),$(TEST_SRCS))

build/$(TARGET_RELEASE): $(OBJS)
	ar -rcs $@ $^

build/$(TARGET_DEBUG): $(DEBUG_OBJS)
	ar -rcs $@ $^

# $(patsubst %.h, , $?) fixes bug in Make which randomly substitutes $? with
# other dependencies. Also touching test source file was the easiest way to
# fool Make to rebuild the corresponding executable with correct dependencies.
$(OBJS): build/%.o : src/%.c
	@mkdir -p build
	$(CC) $(AUTO_GEN_DEPS) -c $(CFLAGS) $(patsubst %.h, ,$?) -o $@
	touch --no-create $(patsubst src/%.c,tests/test_%.c,$?)

$(DEBUG_OBJS): build/%d.o : src/%.c
	@mkdir -p build
	$(CC) $(AUTO_GEN_DEPS) -c $(CFLAGS) $(patsubst %.h, ,$?) -o $@
	touch --no-create $(patsubst src/%.c,tests/test_%.c,$?)

# Use auto-generated header file dependency files
-include $(OBJS:.o=.d)
-include $(DEBUG_OBJS:.o=.d)

$(TEST_EXEC): build/test_%$(EXE_EXT) : tests/test_%.c
	$(CC) $? build/$(TARGET_RELEASE) $(CFLAGS) -o $@

$(TEST_DEBUG_EXEC): build/test_%d$(EXE_EXT) : tests/test_%.c
	$(CC) $? build/$(TARGET_DEBUG) $(CFLAGS) -o $@

run_tests:
	for test in $(TEST_EXEC) ; do \
		./$$test || exit 1 ; \
		echo ; \
	done

run_dtests:
	for test in $(TEST_DEBUG_EXEC) ; do \
		./$$test || exit 1 ; \
		echo ; \
	done

build_tests: $(TEST_EXEC)
tests: MAKEFLAGS =
tests:
	$(MAKE) release -j$(THREAD_COUNT)
	$(MAKE) build_tests -j$(THREAD_COUNT)
	$(MAKE) run_tests -j1

build_dtests: $(TEST_DEBUG_EXEC)
dtests: MAKEFLAGS =
dtests:
	$(MAKE) debug -j$(THREAD_COUNT)
	$(MAKE) build_dtests -j$(THREAD_COUNT)
	$(MAKE) run_dtests -j1

clean:
	rm -rf build

