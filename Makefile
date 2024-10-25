CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
DEBUGFLAGS= -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address
TARGET = main
PHONY = all test graph clean zip debug_scanner

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

obj/error.o: src/error.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/dyn_str.o: src/dyn_str.c 
	$(CC) $(CFLAGS) -c $< -o $@

obj/scanner.o: src/scanner.c obj/dyn_str.o
	$(CC) $(CFLAGS) -c $< -o $@

obj/error-debug.o: src/error.c
	$(CC) $(DEBUGFLAGS) -c $< -o $@

obj/dyn_str-debug.o: src/dyn_str.c 
	$(CC) $(DEBUGFLAGS) -c $< -o $@

obj/scanner-debug.o: src/scanner.c obj/dyn_str.o
	$(CC) $(DEBUGFLAGS) -c $< -o $@

bin/test_scanner: obj/scanner-debug.o obj/dyn_str-debug.o obj/error-debug.o tests/test_utils/scanner_unit_tests.c
	$(CC) $(DEBUGFLAGS) obj/scanner-debug.o obj/dyn_str-debug.o obj/error-debug.o -o bin/test_scanner tests/test_utils/scanner_unit_tests.c

test: bin/test_scanner
	./tests/test.sh

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -f obj/*.o $(TARGET)

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni

debug_scanner: bin/test_scanner
	echo "set debuginfod enabled on\nset args tests/input/test1 tests/output/test1\nbreak main\nbreak get_token\nbreak check_keyword\nrun" > debug_options
	gdb -x debug_options bin/test_scanner
	@rm -f debug_options bin/test_scanner
