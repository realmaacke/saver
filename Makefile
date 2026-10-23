BUILD_DIR := build
MAKEFLAGS += --silent
LINT_CHECKS := -*, \
	performance-*, \
	readability-*, \
	bugprone-*, \
	clang-analyzer-*, \
	cppcoreguidelines-*, \
	misc-*, \
	-readability-convert-member-functions-to-static, \
	-misc-no-recursion, \
	-readability-identifier-length

LINT_COMPILER_ARGS := -Wno-predefined-identifier-outside-function

LINT_FLAGS := -p build -quiet --use-color \
	--header-filter='$(CURDIR)/include/.*|^\.\./include/.*' \
	--checks='$(LINT_CHECKS)' \
	$(addprefix --extra-arg=,$(LINT_COMPILER_ARGS))



.PHONY: all build rebuild run clean local env test test-pipeline linter lint-file

all: build

$(BUILD_DIR)/build.ninja:
	meson setup $(BUILD_DIR)

build: $(BUILD_DIR)/build.ninja
	ninja -C $(BUILD_DIR)

rebuild:
	meson setup --reconfigure $(BUILD_DIR)
	ninja -C $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/saver

clean:
	rm -rf $(BUILD_DIR)

local:
	rm -rf .saver

env:
	rm -rf ~/.local/share/saver

test: build
	meson test -C $(BUILD_DIR) app_unit_tests --print-errorlogs

test-pipeline:
	meson test -C $(BUILD_DIR) app_unit_tests --print-errorlogs

# reguires clang-tidy, i use v21
# install with apt or whatever package manager u use.
linter:
	find src -name '*.cpp' | \
	xargs -P $$(nproc) -n 1 clang-tidy-21 $(LINT_FLAGS) 2>&1 | \
	grep -Ev '^[0-9]+ warnings? generated\.$$' || true

lint-file:
	clang-tidy-21 $(LINT_FLAGS) $(file) 2>&1 |\
	grep -Ev '^[0-9]+ warnings? generated\.$$' || true
