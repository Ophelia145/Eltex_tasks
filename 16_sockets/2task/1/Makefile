listening_server_SRC = listening_server.c
CLIENT_SRC = ../client.c

listening_server_BIN = listening_server
CLIENT_BIN = client
LFLAGS=-Wall -Wextra  -pthread -std=gnu11
#-std=c11
CC=gcc

.PHONY: all rebuild  clean run

all: $(listening_server_BIN) $(CLIENT_BIN)


rebuild: clean style all

$(listening_server_BIN): $(listening_server_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(listening_server_BIN) $(CLIENT_BIN)

run: all
		./listening_server &
		sleep 1
		./client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(listening_server_BIN) ./$(CLIENT_BIN)




style:
		clang-format -i --style=Google *.c