server_SRC = ../server.c
CLIENT_SRC = client.c

server_BIN = server
CLIENT_BIN = client
LFLAGS=-Wall -Wextra  -pthread -std=gnu11
#-std=c11
CC=gcc

.PHONY: all rebuild  clean run

all: $(server_BIN) $(CLIENT_BIN)


rebuild: clean style all

$(server_BIN): $(server_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(server_BIN) $(CLIENT_BIN)

run: all
		./server &
		sleep 1
		./client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(server_BIN) ./$(CLIENT_BIN)




style:
		clang-format -i --style=Google *.c