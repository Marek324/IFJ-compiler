CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address
TARGET = main
PHONY = all clean zip

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(TARGET)

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
