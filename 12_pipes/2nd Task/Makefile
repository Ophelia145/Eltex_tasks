SERVER_SRC = server.c
CLIENT_SRC = client.c

SERVER_BIN = server
CLIENT_BIN = client
LFLAGS=-Wall -Wextra -std=c11 -pthread
CC=gcc

.PHONY: all rebuild  clean run

all: $(SERVER_BIN) $(CLIENT_BIN)


rebuild: clean style all

$(SERVER_BIN): $(SERVER_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(SERVER_BIN) $(CLIENT_BIN)

run: all
		./server &
		sleep 1
		./client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(SERVER_BIN) ./$(CLIENT_BIN)




style:
		clang-format -i --style=Google *.c