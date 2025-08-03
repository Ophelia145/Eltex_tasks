SRC=$(wildcard *.c)
SRCMC = $(filter-out string_from_f_backwards.c, $(wildcard *.c))

OBJ=$(SRCMC:%.c=%.o)
CFLAGS = -Wextra -std=c11 -g3
LDFLAGS = $(shell pkg-config --cflags --libs ncursesw)
CC=gcc
EXECUTABLE=9th_task

.PHONY: all rebuild clean run cpp_clang leaks

all: $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.exe $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)
        
cpp_clang:
	clang-format -i --verbose *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks: $(EXECUTABLE)
	valgrind --leak-check=full ./$(EXECUTABLE)