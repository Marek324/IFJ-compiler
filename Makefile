# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Iinclude -DDEBUG -g 
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

.PHONY: all clean submit tree test zig

all: $(TARGET)

tree:
	d2 tree.d2


test: $(TARGET)
	./test/run.sh 

clean:
	rm -rf $(BUILD_DIR) xhricma00.zip xhricma00 test/out/* $(TARGET) tree.d2 tree.svg aa.out

submit: clean
	mkdir xhricma00
	cp -r src/* include/* docs/rozdeleni docs/rozsireni docs/dokumentace.pdf xhricma00
	cat submit_make > xhricma00/Makefile
	(cd xhricma00 && zip -r ../xhricma00.zip .)
	rm -r xhricma00


$(TARGET): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

S = hello
SRCZIG = codegen_test/src/$(S).zig
zig: $(TARGET)
	@./$(TARGET) < $(SRCZIG) > aa.out
	@codegen_test/ic24int aa.out
