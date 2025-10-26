listening_server_SRC = listening_server.c
CLIENT_SRC = ../client.c
UDP_CLIENT_SRC = udp_client.c

CLIENT_BIN = client
listening_server_BIN = listening_server
UDP_CLIENT_BIN = udp_client

LFLAGS=-Wall -Wextra -std=c11 -pthread
CC=gcc

.PHONY: all rebuild  clean run

all: $(listening_server_BIN) $(CLIENT_BIN) $(UDP_CLIENT_BIN)


rebuild: clean style all

$(listening_server_BIN): $(listening_server_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(UDP_CLIENT_BIN): $(UDP_CLIENT_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(listening_server_BIN) $(CLIENT_BIN) $(UDP_CLIENT_BIN)

run: all
		./listening_server &
		sleep 1
		./client
		./udp_client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(listening_server_BIN) ./$(CLIENT_BIN)




style:
		clang-format -i --style=Google *.c