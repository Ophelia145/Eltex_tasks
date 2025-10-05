CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
TARGET = interface.out

SRC = interface.c driver.c server.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
