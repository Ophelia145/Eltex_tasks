LIB_SRC = add.c sub.c mul.c divi.c
LIB_OBJ=$(LIB_SRC:%.c=%.o)
LFLAGS=-Wall -Wextra -std=c11
CC=gcc
EXECUTABLE=7th_task
LIBRARY = libcalc.a

.PHONY: all rebuild  clean

all: $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)

$(EXECUTABLE): main.c $(LIBRARY)
		$(CC) $(LFLAGS) main.c -L. -lcalc -o $@

$(LIBRARY): $(LIB_OBJ)
		ar rcs $@ $^
%.o: %.c calc.h
		$(CC) $(LFLAGS) -c $< -o $@

clean:
		rm -f *.o *.exe $(EXECUTABLE) $(LIBRARY)

run: $(EXECUTABLE)
		./$(EXECUTABLE)
        
cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(EXECUTABLE)
