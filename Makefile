
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -Wshadow
LDLIBS=-lncurses -ltinfo

BIN=minesweeper.out

.PHONY: all clean

# ------------------------------------------------------------------------------

all: $(BIN)

clean:
	rm -f $(BIN)

# ------------------------------------------------------------------------------

$(BIN): src/*.c src/*.h
	$(CC) $(CFLAGS) -o $@ src/main.c $(LDLIBS)
