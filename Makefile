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

test: scanner.o
	$(CC)  scanner.o src/ScannerTests.c -o test

graph: FSMgraph.dot
	dot -Tpng FSMgraph.dot -o FSMgraph.png

clean:
	rm -f *.o $(TARGET)

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
