CC=gcc
CFLAGS= -Wall -Wextra -Werror --std=c11 -D_POSIX_C_SOURCE=200809L
LDLIBS = -lcheck -lm
GCOV_FLAGS = --coverage
DEBUGFLAGS= -g

OS = $(shell uname -s)
ifeq ($(OS),Linux)
	LDLIBS := $(LDLIBS) -lsubunit
endif

SRC_FILES=$(wildcard ./**/s21*.c)
OBJ_FILES=$(patsubst %.c,%.o, $(SRC_FILES))

TARGET=mcanalog

LOGS_F=reports
HTML_F=html

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET).a test

$(TARGET).a: $(OBJ_FILES)
	ar rc $@ $^
	ranlib $@ # in case ar doesn't have s flag

clean:
	rm -f $(OBJ_FILES)
	rm -f $(TARGET).a

rebuild: clean all

debug: CFLAGS += $(DEBUGFLAGS)
debug: rebuild

memleak: tests/*.c main.c $(TARGET).a
	mkdir -p $(LOGS_F)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $^ -o $(LOGS_F)/$@ $(LDLIBS)
ifeq ($(OS),Darwin)
	leaks -atExit -- ./$(LOGS_F)/$@ > $(LOGS_F)/leaks.log
	grep -n "LEAK:" $(LOGS_F)/leaks.log ||:
else ifeq ($(OS),Linux)
	valgrind --tool=memcheck --leak-check=full --log-file=$(LOGS_F)/valgrind.log ./$(LOGS_F)/$@
	grep -n "ERROR SUMMARY:" $(LOGS_F)/valgrind.log | grep -v "0 errors" ||:
endif
