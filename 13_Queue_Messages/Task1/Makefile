CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS_POSIX = -lrt
INCLUDES = -I.

SRC = $(wildcard *.c)

TARGETS = $(SRC:.c=)

all: $(TARGETS)


server_posix: server_posix.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS_POSIX)

client_posix: client_posix.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS_POSIX)


# для sys V(остальных)
%: %.c
	$(CC) $(CFLAGS) $< -o $@

run-sysv: server_sysV client_sysV
	@./server_sysV & sleep 1; ./client_sysV


run-posix: server_posix client_posix
	@./server_posix  & ./client_posix

rebuild: clean all

run: run-sysv run-posix

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