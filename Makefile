BUILD_DIR := build
MAKEFLAGS += --silent

.PHONY: all build rebuild run clean

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