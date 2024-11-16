# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/common -I$(INCLUDE_DIR)/parser -I$(INCLUDE_DIR)/scanner/ -g
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Source Files
SCANNER_SRCS = $(SRC_DIR)/scanner/scanner.c $(SRC_DIR)/scanner/dyn_str.c $(SRC_DIR)/scanner/circ_buff.c
COMMON_SRCS = $(SRC_DIR)/common/error.c $(SRC_DIR)/common/stack.c $(SRC_DIR)/common/main.c
PARSER_SRCS = $(SRC_DIR)/parser/exp_parser.c $(SRC_DIR)/parser/ast.c $(SRC_DIR)/parser/parser.c

SRCS = $(SCANNER_SRCS) $(COMMON_SRCS) $(PARSER_SRCS)

# Targets
APP_SRCS = $(SRCS)
TEST_SRCS = $(filter-out $(SRC_DIR)/common/main.c, $(SRCS))

APP_OBJS = $(APP_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

APP_TARGET = $(BIN_DIR)/app
TEST_TARGET = $(BIN_DIR)/test

OBJ_SUBDIRS = $(OBJ_DIR)/common $(OBJ_DIR)/scanner $(OBJ_DIR)/parser

# Default Target
all: app

# Application Target
app: $(APP_OBJS) | $(OBJ_SUBDIRS) $(BIN_DIR)
	$(CC) $(CFLAGS) $(APP_OBJS) -o $(APP_TARGET) $(LDFLAGS)

run: app
	$(APP_TARGET)

# Test Target
test: $(TEST_TARGET)
	@echo "Running tests..."
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) | $(OBJ_SUBDIRS) $(BIN_DIR)
	$(CC) $(CFLAGS) $(TEST_OBJS) tests/test_utils/scanner_unit_tests.c -o $(TEST_TARGET) $(LDFLAGS)

# Object File Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_SUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Directory Creation
$(OBJ_SUBDIRS) $(BIN_DIR):
	mkdir -p $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR)

# Phony Targets
.PHONY: all app run test clean
