
CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lncurses -ltinfo
BIN=minesweeper.out

.PHONY: clean all run

# -------------------------------------------

all: $(BIN)

run: $(BIN)
	./$<

clean:
	rm -f $(BIN)

# -------------------------------------------

$(BIN): src/*.c src/*.h
	$(CC) $(CFLAGS) -o $@ src/main.c $(LDFLAGS)

