server_SRC = server.c
CLIENT_SRC = client.c

server_BIN = server.out
CLIENT_BIN = client.out
LFLAGS = -Wall -Wextra -pthread -std=gnu11
CC = gcc

.PHONY: all clean

all: $(server_BIN) $(CLIENT_BIN)

$(server_BIN): $(server_SRC)
	$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(LFLAGS) -o $@ $^

clean:
	rm -f $(server_BIN) $(CLIENT_BIN)
