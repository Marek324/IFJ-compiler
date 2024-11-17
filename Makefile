# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude -g -fsanitize=address
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/main.c, $(SRCS))
# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN = $(SRC_DIR)/main.c
TARGET = ifj24compiler

.PHONY: all clean purge submit

all: $(TARGET)

test: tests/TestsCheck.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@ -lcheck -lsubunit -lm
	./test
	@rm -f test

clean:
	rm -rf $(BUILD_DIR) xhricma00.zip xhricma00

purge: clean
	rm -f $(TARGET) test

submit: purge
	mkdir xhricma00
	cp -r src/* include/* docs/rozdeleni docs/rozsireni docs/dokumentace.pdf xhricma00
	cat submit_make > xhricma00/Makefile
	zip -r xhricma00.zip xhricma00/*

$(TARGET): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@
