CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address
TARGET = main
PHONY = all clean doxygen zip

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(TARGET)

doxygen:
	doxygen Doxyfile

zip:
	zip xhricma00.zip *.c *.h Makefile rozdeleni
