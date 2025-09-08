CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS_POSIX = -lrt
INCLUDES = -I.

SRC = $(wildcard *.c)

TARGETS = $(SRC:.c=)

all: $(TARGETS)


server: server.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS_POSIX)

client: client.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS_POSIX)


run-server: server
	./server

run-client: client
	./client

rebuild: clean all

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(EXECUTABLE)


style:
		clang-format -i --style=Google *.c