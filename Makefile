# -------------------------
# Toolchain
# -------------------------
CXX := clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pthread -Iinclude -I.
CXXFLAGS += -I/opt/homebrew/include
LDFLAGS  += -L/opt/homebrew/lib

# -------------------------
# Directories & files
# -------------------------
BIN_DIR := bin
OBJ_DIR := obj

SRCS := $(wildcard src/*.cpp)
SRCS_OBJS := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
MAIN_OBJ := $(OBJ_DIR)/main.o
APP_OBJS := $(filter-out $(MAIN_OBJ),$(SRCS_OBJS))

TEST_SRCS := $(wildcard tests/*.cpp)
TEST_OBJS := $(patsubst tests/%.cpp,$(OBJ_DIR)/tests_%.o,$(TEST_SRCS))

TARGET := $(BIN_DIR)/stock_market
TEST_TARGET := $(BIN_DIR)/test_stock_market

CONFIG ?= trading_hours.ini

GTEST_LIBS ?= -lgtest -lgtest_main -pthread

# -------------------------
# Default target
# -------------------------
.PHONY: all
all: build

# -------------------------
# Build main executable
# -------------------------
.PHONY: build
build: $(TARGET)

$(TARGET): $(SRCS_OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS_OBJS) $(LDFLAGS)

# -------------------------
# Build and run tests
# -------------------------
.PHONY: test
test: $(TEST_TARGET)
	@echo "[RUN TESTS] $(TEST_TARGET) $(CONFIG)"
	@$(TEST_TARGET) "$(CONFIG)"

$(TEST_TARGET): $(APP_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	@echo "[LD] $@"
	$(CXX) $(CXXFLAGS) -o $@ $(APP_OBJS) $(TEST_OBJS) $(GTEST_LIBS) $(LDFLAGS)

# -------------------------
# Object rules
# -------------------------
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	@echo "[CC] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/tests_%.o: tests/%.cpp | $(OBJ_DIR)
	@echo "[CC] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Create directories
# -------------------------
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# -------------------------
# Run main program
# -------------------------
.PHONY: run
run: build
	@echo "[RUN] $(TARGET) $(CONFIG)"
	@$(TARGET) "$(CONFIG)"

# -------------------------
# Lint
# -------------------------
.PHONY: lint
lint:
	@if command -v clang-tidy >/dev/null 2>&1; then \
	  for f in $(SRCS) $(TEST_SRCS); do \
	    clang-tidy $$f -- $(CXXFLAGS) || true; \
	  done; \
	elif command -v cpplint >/dev/null 2>&1; then \
	  cpplint $(SRCS) $(TEST_SRCS); \
	else \
	  echo "No clang-tidy or cpplint found in PATH."; \
	  exit 1; \
	fi

# -------------------------
# Clean
# -------------------------
.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
