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

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -rf $(OBJ)/* $(BIN)/* $(TEST)/bin/*

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni

#usage make debug_scanner test=test_name
debug_scanner: bin/test_scanner
	echo "set debuginfod enabled on\nbreak main\nbreak get_token\nrun < tests/input/scanner/$(test) > tests/output/scanner/$(test) " > debug_options
	gdb -x debug_options bin/test_scanner
	@rm -f debug_options bin/test_scanner
