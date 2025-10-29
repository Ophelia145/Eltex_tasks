multicast_server_SRC = multicast_server.c
multicast_client_SRC = multicast_client.c

multicast_server_BIN = multicast_server
multicast_client_BIN = multicast_client
LFLAGS=-Wall -Wextra  -pthread -std=gnu11
#-std=c11
CC=gcc

.PHONY: all rebuild  clean run

all: $(multicast_server_BIN) $(multicast_client_BIN)


rebuild: clean style all

$(multicast_server_BIN): $(multicast_server_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(multicast_client_BIN): $(multicast_client_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(multicast_server_BIN) $(multicast_client_BIN)

run: all
		./multicast_server &
		sleep 1
		./multicast_client

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(multicast_server_BIN) ./$(multicast_client_BIN)




style:
		clang-format -i --style=Google *.c