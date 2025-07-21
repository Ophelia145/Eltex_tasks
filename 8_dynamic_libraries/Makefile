LIB_SRC = add.c sub.c mul.c divi.c
LIB_OBJ = $(LIB_SRC:.c=.o)  
LFLAGS = -Wall -Wextra -std=c11 -fPIC -fvisibility=hidden
#we hide inner symbols
CC = gcc
EXECUTABLE = 8th_task
LIBRARY = libcalc.so

.PHONY: all rebuild clean run cpp_clang leaks

all: $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)

$(EXECUTABLE): main.c $(LIBRARY)
	$(CC) main.c -L. -lcalc -o $@

$(LIBRARY): $(LIB_OBJ)
	$(CC) -shared -o $@ $^  

%.o: %.c calc.h
	$(CC) $(LFLAGS) -c $< -o $@

clean:
	rm -f *.o *.exe $(EXECUTABLE) $(LIBRARY)

run: $(EXECUTABLE)
	LD_LIBRARY_PATH=. ./$(EXECUTABLE) 

cpp_clang:
	clang-format -i --verbose *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks: $(EXECUTABLE)
	LD_LIBRARY_PATH=. valgrind --leak-check=full ./$(EXECUTABLE)