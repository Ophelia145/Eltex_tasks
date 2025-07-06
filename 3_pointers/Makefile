SRC=$(wildcard *.c)
OBJS = main.o 1.o 3.o 4.o
CFLAGS = -Wall -Wextra -std=c11
CC=gcc
EXECUTABLE=3rd_task
EX2=2

.PHONY: all rebuild run run2 clean cpp_clang leaks

all: $(EXECUTABLE) $(EX2)

run2: $(EX2)
	./$(EX2)

2.o: 2.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EX2): 2.o
	$(CC) -o $@ $^


$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^

rebuild: clean all

clean:
	rm -f *.o *.exe $(EXECUTABLE) $(EX2)

run: $(EXECUTABLE)
	./$(EXECUTABLE)
        
cpp_clang:
	clang-format -i --verbose *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks: $(EXECUTABLE)
	valgrind --leak-check=full ./$(EXECUTABLE)