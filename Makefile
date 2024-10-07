CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address
TARGET = main
PHONY = all clean graph zip

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

scanner.o:
	$(CC) -c -o scanner.o src/scanner.c

dyn_str.o:
	$(CC) -c -o dyn_str.o src/dyn_str.c

error.o:
	$(CC) -c -o error.o src/error.c

test: scanner.o dyn_str.o error.o
	$(CC)  error.o scanner.o dyn_str.o src/ScannerTests.c -o test -I/usr/local/include -lcmocka

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -f *.o $(TARGET)

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
