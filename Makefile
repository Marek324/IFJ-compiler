CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address
TARGET = main
PHONY = all test graph clean zip

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

test: obj/scanner.o obj/dyn_str.o obj/error.o tests/test_utils/scanner_unit_tests.c
	$(CC) $(CFLAGS) obj/scanner.o obj/dyn_str.o obj/error.o -o $@ tests/test_utils/scanner_unit_tests.c
	./tests/test.sh
	rm -f test

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -f *.o $(TARGET)

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
