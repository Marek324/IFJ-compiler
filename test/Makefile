CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude

TESTARG = $(word 2, $(MAKECMDGOALS))

#directories
SRC = src
OBJ = obj
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BIN = bin
INC = include

TEST = test
TESTS = $(wildcard $(TEST)/*.c)
TESTBINS = $(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(TESTS))

$(OBJ)/%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $@

$(TEST)/bin/%: $(TEST)/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@  -lcheck -lsubunit -lm

$(TEST)/bin:
	mkdir -p $@

test: $(TEST)/bin $(TESTBINS)
	for test in $(TESTBINS); do $$test $(TESTARG); done

clean:
	rm -rf $(OBJ)/* $(BIN)/* $(TEST)/bin/*
