CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -g -Iinclude
LDFLAGS :=
LDLIBS := -lcurl

BUILD_DIR := build
TARGET := $(BUILD_DIR)/saver
MAKEFLAGS += --silent

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)