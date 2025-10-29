broadcast_server_SRC = broadcast_server.c
CLIENT_SRC = broadcast_client.c

broadcast_server_BIN = broadcast_server
CLIENT_BIN = broadcast_client
LFLAGS=-Wall -Wextra  -pthread -std=gnu11
#-std=c11
CC=gcc

.PHONY: all rebuild  clean run

all: $(broadcast_server_BIN) $(CLIENT_BIN)


rebuild: clean style all

$(broadcast_server_BIN): $(broadcast_server_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^



clean:
		rm -f $(broadcast_server_BIN) $(CLIENT_BIN)

run: all
		./broadcast_server &
		sleep 1
		./client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(broadcast_server_BIN) ./$(CLIENT_BIN)




style:
		clang-format -i --style=Google *.c