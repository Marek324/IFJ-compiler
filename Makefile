CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
DEBUGFLAGS= -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address
TARGET = main
PHONY = all test graph clean zip debug_scanner

SRC = src
OBJ = obj
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN = bin
INC = include

TEST = test
TESTS = $(wildcard $(TEST)/*.c)
TESTBINS = $(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(TESTS))
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

obj/error.o: src/error.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/dyn_str.o: src/dyn_str.c 
	$(CC) $(CFLAGS) -c $< -o $@

obj/circ_buff.o: src/circ_buff.c 
	$(CC) $(CFLAGS) -c $< -o $@

obj/scanner.o: src/scanner.c obj/dyn_str.o
	$(CC) $(CFLAGS) -c $< -o $@

obj/codegen.o: src/codegen.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/error-debug.o: src/error.c
	$(CC) $(DEBUGFLAGS) -c $< -o $@

obj/dyn_str-debug.o: src/dyn_str.c 
	$(CC) $(DEBUGFLAGS) -c $< -o $@

obj/circ_buff-debug.o: src/circ_buff.c 
	$(CC) $(DEBUGFLAGS) -c $< -o $@

obj/scanner-debug.o: src/scanner.c 
	$(CC) $(DEBUGFLAGS) -c $< -o $@

bin/test_scanner: obj/scanner-debug.o obj/dyn_str-debug.o obj/circ_buff-debug.o obj/error-debug.o tests/test_utils/scanner_unit_tests.c
	$(CC) $(DEBUGFLAGS) obj/scanner-debug.o obj/dyn_str-debug.o obj/circ_buff-debug.o obj/error-debug.o -o bin/test_scanner tests/test_utils/scanner_unit_tests.c

test: bin/test_scanner
	./tests/test.sh

codegen: obj/codegen.o
	$(CC) $(CFLAGS) $^ -o ./bin/codegen
	@./bin/codegen > test_test_test/test.out
	@test_test_test/ic24int test_test_test/test.out < test_test_test/test.in

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -rf $(OBJ)/* $(BIN)/* $(TEST)/bin/*

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
	