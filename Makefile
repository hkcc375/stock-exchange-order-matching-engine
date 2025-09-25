# Compiler and flags
CXX := g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pthread -Iinclude -I.

# Directories and target names
BIN_DIR := bin
OBJ_DIR := obj
TARGET := $(BIN_DIR)/stock_market
TEST_TARGET := $(BIN_DIR)/test_stock_market

# Google Test libraries (assumes installed or built)
GTEST_LIBS := -lgtest_main -lgtest -pthread

# Configuration file
CONFIG ?= trading_hours.ini

# Find all .cpp source files in src/
SRCS := $(wildcard src/*.cpp)
# (Optional) error out if no sources found
ifeq ($(strip $(SRCS)),)
  $(error No source files found! Did you copy src/ correctly?)
endif

# Convert source file names to object file paths
SRCS_OBJS := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
# Identify main.o if main.cpp exists
MAIN_OBJ := $(OBJ_DIR)/main.o
# All other objects (for linking tests without main)
APP_OBJS := $(filter-out $(MAIN_OBJ),$(SRCS_OBJS))

# Find all test source files in tests/
TEST_SRCS := $(wildcard tests/*.cpp)
TEST_OBJS := $(patsubst tests/%.cpp,$(OBJ_DIR)/tests_%.o,$(TEST_SRCS))

.PHONY: all
all: build

# Build the main application
.PHONY: build
build: $(TARGET)

$(TARGET): $(SRCS_OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS_OBJS) $(LDFLAGS)

# Build and run tests
.PHONY: test
test: $(TEST_TARGET)
	@echo "[RUN TESTS] $(TEST_TARGET) $(CONFIG)"
	@./$(TEST_TARGET) "$(CONFIG)"

$(TEST_TARGET): $(APP_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(APP_OBJS) $(TEST_OBJS) $(GTEST_LIBS)

# Compile objects from src/ and tests/
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	@echo "[CC] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/tests_%.o: tests/%.cpp | $(OBJ_DIR)
	@echo "[CC] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure bin/ and obj/ directories exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Run the main program
.PHONY: run
run: build
	@echo "[RUN] $(TARGET) $(CONFIG)"
	@./$(TARGET) "$(CONFIG)"

# Clean all build artifacts
.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
