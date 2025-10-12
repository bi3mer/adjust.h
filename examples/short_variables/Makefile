CC = cc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -g -DMODE_DEBUG
LDFLAGS =
LDLIBS =
INCLUDES = -I../  # For adjust.h
SOURCES = $(wildcard *.c)
EXECUTABLES = $(SOURCES:%.c=build_%)

all: $(EXECUTABLES)

# Build specific target with: make build_filename
build_%: %.c
	@echo "Building $@..."
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS) $(LDLIBS)

# Alternative: make filename (creates build_filename)
%: %.c
	@echo "Building build_$@..."
	$(CC) $(CFLAGS) $(INCLUDES) $< -o build_$@ $(LDFLAGS) $(LDLIBS)

clean:
	@rm -rf build_*

run-%: build_%
	@echo "Running build_$*..."
	@./build_$*

run-all: all
	@for exe in $(EXECUTABLES); do \
		echo "Running $$exe..."; \
		./$$exe; \
		echo "---"; \
	done

info:
	@echo "Source files:"
	@for src in $(SOURCES); do echo "  $$src"; done
	@echo ""
	@echo "Will build:"
	@for exe in $(EXECUTABLES); do echo "  $$exe"; done

.PHONY: all clean run-all info