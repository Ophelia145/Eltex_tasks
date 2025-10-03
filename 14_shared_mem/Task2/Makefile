CC = gcc
CFLAGS = -Wall -Wextra -g -std=gnu11
LDFLAGS_POSIX = -lrt

SRC = $(wildcard *.c)

TARGETS = $(SRC:.c=)

all: $(TARGETS)


server: server.c
	$(CC) $(CFLAGS) -I. $< -o $@ $(LDFLAGS_POSIX)

client: client.c
	$(CC) $(CFLAGS) -I. $< -o $@ $(LDFLAGS_POSIX)  -lncurses


run-server: server
	./server

run-client: client
	./client

rebuild: clean all

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean

leaks:  
		valgrind --leak-check=full ./server ./client

style:
		clang-format -i --style=Google *.c