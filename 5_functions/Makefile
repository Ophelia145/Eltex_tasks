LFLAGS=-Wall -Wextra -std=c11
GBDFLAGS=-fno-stack-protector -no-pie
CC=gcc

.PHONY: all rebuild main cracker clean

all: main cracker

rebuild: clean all

cracker: cracker.o
		$(CC) $(LFLAGS) -o cracker cracker.o

cracker.o: cracker.c
		$(CC) $(LFLAGS) -c -o $@ $^

main: main.o
		$(CC) $(GBDFLAGS) $(LFLAGS)  -o main main.o

main.o: main.c
		$(CC) $(GBDFLAGS) $(LFLAGS) -c -o $@ $^

clean:
		rm -f *.o *.exe main cracker

run: cracker main 
		./cracker
		./main < fill.txt
        
cpp_clang:
		clang-format -i --verbose *.c *.h
		cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

leaks:  
		valgrind --leak-check=full ./main
