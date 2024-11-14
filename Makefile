CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude -Iinclude/parser -g -fsanitize=address
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
INCLUDE_DIR = include

SCANNER_SRCS = $(SRC_DIR)/scanner/scanner.c $(SRC_DIR)/scanner/dyn_str.c $(SRC_DIR)/scanner/circ_buff.c
COMMON_SRCS = $(SRC_DIR)/common/error.c $(SRC_DIR)/common/stack.c
PARSER_SRCS = $(SRC_DIR)/parser/exp_parser.c $(SRC_DIR)/parser/ast.c

SCANNER_OBJS = $(SCANNER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
COMMON_OBJS = $(COMMON_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
PARSER_OBJS = $(PARSER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

OBJS = $(SCANNER_OBJS) $(COMMON_OBJS) $(PARSER_OBJS)

#TARGET = $(BIN_DIR)/main
TARGET = $(BIN_DIR)/test

OBJ_SUBDIRS = $(OBJ_DIR)/common $(OBJ_DIR)/scanner $(OBJ_DIR)/parser

all: $(TARGET)

$(TARGET): $(OBJS) | $(OBJ_SUBDIRS) $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) tests/test_utils/scanner_unit_tests.c -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_SUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_SUBDIRS) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

test: $(TARGET)
	@echo "Running tests..."

.PHONY: all clean test
