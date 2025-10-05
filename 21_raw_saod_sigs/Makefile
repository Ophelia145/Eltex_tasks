CC=gcc
CFLAGS= -Wall -Wextra
DEBUGFLAGS= -g

all: server.out client.out

server.out: server.c
	$(CC) $(CFLAGS) $^ -o $@

client.out: client.c	
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f server.out client.out
