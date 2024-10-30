CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude -g -fsanitize=address
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
INCLUDE_DIR = include

SCANNER_SRCS = $(SRC_DIR)/scanner/scanner.c $(SRC_DIR)/scanner/dyn_str.c $(SRC_DIR)/scanner/circ_buff.c
COMMON_SRCS = $(SRC_DIR)/common/error.c

SCANNER_OBJS = $(SCANNER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
COMMON_OBJS = $(COMMON_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

OBJS = $(SCANNER_OBJS) $(COMMON_OBJS)

#TARGET = $(BIN_DIR)/main
TARGET = $(BIN_DIR)/test

OBJ_SUBDIRS = $(OBJ_DIR)/common $(OBJ_DIR)/scanner

all: $(TARGET)

$(TARGET): $(OBJS) |$(OBJ_SUBDIRS) $(BIN_DIR)
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
