CC=gcc
# LFLAGS=-Wall -Wextra -std=c11 -pthread

SENDER_SRC = sender.c
GETTER_SRC = getter.c

SENDER_BIN = sender
GETTER_BIN = getter


.PHONY: all rebuild  clean run

all: $(SENDER_BIN) $(GETTER_BIN)


rebuild: clean style all

$(SENDER_BIN): $(SENDER_SRC)
		$(CC) $(LFLAGS) -o $@ $^

$(GETTER_BIN): $(GETTER_SRC)
		$(CC) $(LFLAGS) -o $@ $^


clean:
		rm -f $(SENDER_BIN) $(GETTER_BIN)

# run_send:
# 		./$(SENDER_BIN)
# run_get:
# 		./$(GETTER_BIN)

cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./$(SENDER_BIN) ./$(GETTER_BIN)

style:
		clang-format -i --style=Google *.c