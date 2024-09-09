CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address
SRCS = main.c
OBJS = $(SRCS:.c=.o)
TARGET = main
PHONY = all clean

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(TARGET)