SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=%.o)
LFLAGS=-Wall -Wextra -std=c11
CC=gcc
EXECUTABLE=1st_task

.PHONY: all rebuild $(EXECUTABLE) clean

all: 1st_task

rebuild: clean $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
		$(CC) $(LFLAGS) -o $(EXECUTABLE) $(OBJ)

%.o: %.c
		$(CC) $(LFLAGS) -c -o $@ $^

clean:
		rm -f *.o *.exe $(EXECUTABLE)

run: $(EXECUTABLE)
		./$(EXECUTABLE)
        
cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(EXECUTABLE)
